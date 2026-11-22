#include <rtlib.h>
#include <thread_manager.h>

// The headers of the system come after those of april (windows.h defines macros that the others use: near, far, interface).
#ifdef _WIN32
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0A00
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <poll.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#include <cerrno>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // The functions of System.Net.Sockets: TCP over IPv4. The sockets do not wait: a call that cannot go on returns the code
    // would_block, and the library of the language waits for the socket (Socket_Poll, called by the thread of the reactor, or by the
    // thread that waits for it) and calls again. A function that can fail returns a negative code: -1 would block (also: the connection
    // is being made), -2 refused, -3 the connection was reset or closed, -4 any other error, -5 the address is in use, -6 timed out.
    // A call that waits for the system gives the lock of the runtime back (thread_manager_t::blocking).

    namespace
    {
    #ifdef _WIN32
        typedef SOCKET      __socket_t;
        const __socket_t    __invalid = INVALID_SOCKET;

        int __last_error() { return WSAGetLastError(); }
        void __close_socket(__socket_t s) { closesocket(s); }

        bool __is_would_block(int e) { return e == WSAEWOULDBLOCK || e == WSAEINPROGRESS || e == WSAEALREADY; }

        // Starts the library of sockets (once).
        void __start()
        {
            static std::once_flag once;
            std::call_once(once, [] { WSADATA data; WSAStartup(MAKEWORD(2, 2), &data); });
        }

        void __set_non_blocking(__socket_t s)
        {
            u_long on = 1;
            ioctlsocket(s, FIONBIO, &on);
        }

        typedef WSAPOLLFD   __pollfd_t;
        int __poll(__pollfd_t * fds, int count, int timeout) { return WSAPoll(fds, (ULONG)count, timeout); }

        const int __e_refused = WSAECONNREFUSED, __e_use = WSAEADDRINUSE, __e_timeout = WSAETIMEDOUT;

        bool __is_reset(int e)
        {
            return e == WSAECONNRESET || e == WSAECONNABORTED || e == WSAENOTCONN || e == WSAESHUTDOWN || e == WSAENETRESET
                   || e == WSAENOTSOCK;
        }
    #else
        typedef int         __socket_t;
        const __socket_t    __invalid = -1;

        int __last_error() { return errno; }
        void __close_socket(__socket_t s) { close(s); }

        bool __is_would_block(int e) { return e == EWOULDBLOCK || e == EAGAIN || e == EINPROGRESS || e == EALREADY; }

        void __start() { }

        void __set_non_blocking(__socket_t s)
        {
            fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
        }

        typedef struct pollfd __pollfd_t;
        int __poll(__pollfd_t * fds, int count, int timeout) { return poll(fds, (nfds_t)count, timeout); }

        const int __e_refused = ECONNREFUSED, __e_use = EADDRINUSE, __e_timeout = ETIMEDOUT;

        bool __is_reset(int e)
        {
            return e == ECONNRESET || e == ECONNABORTED || e == ENOTCONN || e == EPIPE || e == ENETRESET || e == EBADF;
        }
    #endif

        const int __would_block = -1, __refused = -2, __reset = -3, __failed = -4, __in_use = -5, __timed_out = -6;

        // The code of an error of the system.
        int __code_of(int error)
        {
            if (__is_would_block(error))
                return __would_block;

            if (error == __e_refused)
                return __refused;

            if (error == __e_use)
                return __in_use;

            if (error == __e_timeout)
                return __timed_out;

            return __is_reset(error)? __reset : __failed;
        }

        struct __handle_args_t
        {
            rint64_t    handle;
        };

        struct __address_args_t
        {
            rint64_t    handle;
            rstring_t   address;
            rint32_t    port;
        };

        struct __int_args_t
        {
            rint64_t    handle;
            rint32_t    value;
        };

        struct __option_args_t
        {
            rint64_t    handle;
            rint32_t    option;
            rint32_t    value;
        };

        struct __io_args_t
        {
            rint64_t    handle;
            rptr_t      buffer;
            rint32_t    count;
        };

        struct __poll_args_t
        {
            rptr_t      handles;        // Int64 []
            rptr_t      events;         // Int32 []: 1 to read, 2 to write
            rptr_t      results;        // Int32 []: 1 can read, 2 can write, 4 error or closed
            rint32_t    count;
            rint32_t    timeout;        // Milliseconds, -1 is for ever.
        };

        struct __host_args_t
        {
            rstring_t   host;
        };

        std::string __narrow(const rt_char_t * s)
        {
            std::wstring text = s == nullptr? std::wstring() : to_host_string(s, rt_string_length(s));

            std::string result;
            for (wchar_t c : text)
                result += (char)c;

            return result;
        }

        bool __address_of(const rt_char_t * text, int port, sockaddr_in & address)
        {
            std::memset(&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            address.sin_port = htons((unsigned short)port);

            return inet_pton(AF_INET, __narrow(text).c_str(), &address.sin_addr) == 1;
        }

        template<typename f_t> void __blocking(f_t f)
        {
            thread_manager_t::instance().blocking(f);
        }

        // The socket that wakes up the thread that waits in Socket_Poll: a UDP socket of the loopback that receives a byte.
        std::mutex      __wake_mutex;
        __socket_t      __wake_receiver = __invalid, __wake_sender = __invalid;
        sockaddr_in     __wake_address;
    }

    ////////// ////////// ////////// ////////// //////////

    // Socket_Create(): the handle of a new TCP socket (that does not wait), or the code.
    void socket_create(rtlib_context_t & ctx)
    {
        __start();

        __socket_t s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == __invalid)
        {
            ctx.return_<int64_t>(__code_of(__last_error()));
            return;
        }

        __set_non_blocking(s);
        ctx.return_<int64_t>((int64_t)s);
    }

    // Socket_Bind(handle, address, port).
    void socket_bind(rtlib_context_t & ctx)
    {
        __address_args_t * args = ctx.args<__address_args_t>();
        sockaddr_in address;

        int result = 0;
        if (!__address_of(*args->address, *args->port, address))
            result = __failed;
        else if (::bind((__socket_t)*args->handle, (sockaddr *)&address, sizeof(address)) != 0)
            result = __code_of(__last_error());

        ctx.return_<int32_t>(result);
    }

    // Socket_Listen(handle, backlog).
    void socket_listen(rtlib_context_t & ctx)
    {
        __int_args_t * args = ctx.args<__int_args_t>();
        ctx.return_<int32_t>(::listen((__socket_t)*args->handle, *args->value) == 0? 0 : __code_of(__last_error()));
    }

    // Socket_Accept(handle): the handle of the connection, or the code (would block: no one waits).
    void socket_accept(rtlib_context_t & ctx)
    {
        __socket_t s = ::accept((__socket_t)*ctx.args<__handle_args_t>()->handle, nullptr, nullptr);
        if (s == __invalid)
        {
            ctx.return_<int64_t>(__code_of(__last_error()));
            return;
        }

        __set_non_blocking(s);
        ctx.return_<int64_t>((int64_t)s);
    }

    // Socket_Connect(handle, address, port): 0 when it is made, would block when it is being made (wait for the socket to be
    // writable, then Socket_FinishConnect), or the code.
    void socket_connect(rtlib_context_t & ctx)
    {
        __address_args_t * args = ctx.args<__address_args_t>();
        sockaddr_in address;

        int result = 0;
        if (!__address_of(*args->address, *args->port, address))
            result = __failed;
        else if (::connect((__socket_t)*args->handle, (sockaddr *)&address, sizeof(address)) != 0)
            result = __code_of(__last_error());

        ctx.return_<int32_t>(result);
    }

    // Socket_FinishConnect(handle): 0 when the connection was made, or the code of what failed.
    void socket_finish_connect(rtlib_context_t & ctx)
    {
        int error = 0;
        socklen_t length = sizeof(error);

        int result = 0;
        if (getsockopt((__socket_t)*ctx.args<__handle_args_t>()->handle, SOL_SOCKET, SO_ERROR, (char *)&error, &length) != 0)
            result = __code_of(__last_error());
        else if (error != 0)
            result = __code_of(error);

        ctx.return_<int32_t>(result);
    }

    // Socket_Send(handle, buffer, count): the bytes that were sent, or the code.
    void socket_send(rtlib_context_t & ctx)
    {
        __io_args_t * args = ctx.args<__io_args_t>();

        int n = (int)::send((__socket_t)*args->handle, (const char *)*args->buffer, *args->count, 0);
        ctx.return_<int32_t>(n >= 0? n : __code_of(__last_error()));
    }

    // Socket_Receive(handle, buffer, count): the bytes that were read (0: the other side closed), or the code.
    void socket_receive(rtlib_context_t & ctx)
    {
        __io_args_t * args = ctx.args<__io_args_t>();

        int n = (int)::recv((__socket_t)*args->handle, (char *)*args->buffer, *args->count, 0);
        ctx.return_<int32_t>(n >= 0? n : __code_of(__last_error()));
    }

    // Socket_Shutdown(handle, how): 0 receive, 1 send, 2 both.
    void socket_shutdown(rtlib_context_t & ctx)
    {
        __int_args_t * args = ctx.args<__int_args_t>();
        int how = *args->value;

    #ifdef _WIN32
        int mode = how == 0? SD_RECEIVE : how == 1? SD_SEND : SD_BOTH;
    #else
        int mode = how == 0? SHUT_RD : how == 1? SHUT_WR : SHUT_RDWR;
    #endif

        ctx.return_<int32_t>(::shutdown((__socket_t)*args->handle, mode) == 0? 0 : __code_of(__last_error()));
    }

    // Socket_Close(handle).
    void socket_close(rtlib_context_t & ctx)
    {
        __close_socket((__socket_t)*ctx.args<__handle_args_t>()->handle);
    }

    // Socket_LocalPort(handle), Socket_RemotePort(handle): the port, or the code.
    static void __port_of(rtlib_context_t & ctx, bool remote)
    {
        __socket_t s = (__socket_t)*ctx.args<__handle_args_t>()->handle;

        sockaddr_in address;
        socklen_t length = sizeof(address);
        int r = remote? getpeername(s, (sockaddr *)&address, &length) : getsockname(s, (sockaddr *)&address, &length);

        ctx.return_<int32_t>(r == 0? (int32_t)ntohs(address.sin_port) : __code_of(__last_error()));
    }

    void socket_local_port(rtlib_context_t & ctx)
    {
        __port_of(ctx, false);
    }

    void socket_remote_port(rtlib_context_t & ctx)
    {
        __port_of(ctx, true);
    }

    // Socket_RemoteAddress(handle): the address of the other side as text (null if it is not known).
    void socket_remote_address(rtlib_context_t & ctx)
    {
        __socket_t s = (__socket_t)*ctx.args<__handle_args_t>()->handle;

        sockaddr_in address;
        socklen_t length = sizeof(address);

        char text[64];
        if (getpeername(s, (sockaddr *)&address, &length) != 0 || inet_ntop(AF_INET, &address.sin_addr, text, sizeof(text)) == nullptr)
        {
            ctx.return_<const rt_char_t *>(nullptr);
            return;
        }

        ctx.return_<const rt_char_t *>(new_rt_string(std::wstring(text, text + std::strlen(text))));
    }

    // Socket_SetOption(handle, option, value): 1 NoDelay, 2 ReuseAddress, 3 KeepAlive.
    void socket_set_option(rtlib_context_t & ctx)
    {
        __option_args_t * args = ctx.args<__option_args_t>();
        __socket_t s = (__socket_t)*args->handle;
        int value = *args->value;

        int r;
        switch (*args->option)
        {
            case 1:     r = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *)&value, sizeof(value));  break;
            case 2:     r = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&value, sizeof(value));  break;
            case 3:     r = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *)&value, sizeof(value));  break;
            default:    r = -1; break;
        }

        ctx.return_<int32_t>(r == 0? 0 : __code_of(__last_error()));
    }

    // Socket_Poll(handles, events, results, count, timeout): waits until a socket can be read or written, or the time (milliseconds,
    // -1 is for ever) is over; returns how many are ready. The lock of the runtime is not held while it waits.
    void socket_poll(rtlib_context_t & ctx)
    {
        __poll_args_t * args = ctx.args<__poll_args_t>();
        int64_t * handles = (int64_t *)*args->handles;
        int32_t * events = (int32_t *)*args->events;
        int32_t * results = (int32_t *)*args->results;
        int count = *args->count;
        int timeout = *args->timeout;

        __start();

        std::vector<__pollfd_t> fds((size_t)count);
        for (int k = 0; k < count; k++)
        {
            fds[k].fd = (__socket_t)handles[k];
            fds[k].events = (short)(((events[k] & 1) != 0? POLLIN : 0) | ((events[k] & 2) != 0? POLLOUT : 0));
            fds[k].revents = 0;
        }

        int ready = 0;
        __blocking([&] { ready = count == 0? 0 : __poll(fds.data(), count, timeout); });

        if (ready < 0)
        {
            ctx.return_<int32_t>(__code_of(__last_error()));
            return;
        }

        for (int k = 0; k < count; k++)
        {
            short r = fds[k].revents;
            results[k] = ((r & POLLIN) != 0? 1 : 0) | ((r & POLLOUT) != 0? 2 : 0) | ((r & (POLLERR | POLLHUP | POLLNVAL)) != 0? 4 : 0);
        }

        ctx.return_<int32_t>(ready);
    }

    // Socket_WakeHandle(): the handle of the socket that Socket_Wake sends a byte to (a thread that waits with it in Socket_Poll goes
    // on; Socket_Receive on it takes the bytes away).
    void socket_wake_handle(rtlib_context_t & ctx)
    {
        std::lock_guard<std::mutex> lock(__wake_mutex);

        if (__wake_receiver == __invalid)
        {
            __start();

            __wake_receiver = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            __wake_sender = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            std::memset(&__wake_address, 0, sizeof(__wake_address));
            __wake_address.sin_family = AF_INET;
            __wake_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            socklen_t length = sizeof(__wake_address);
            if (__wake_receiver == __invalid || __wake_sender == __invalid
                    || ::bind(__wake_receiver, (sockaddr *)&__wake_address, sizeof(__wake_address)) != 0
                    || getsockname(__wake_receiver, (sockaddr *)&__wake_address, &length) != 0)
            {
                ctx.return_<int64_t>(__failed);
                return;
            }

            __set_non_blocking(__wake_receiver);
        }

        ctx.return_<int64_t>((int64_t)__wake_receiver);
    }

    // Socket_Wake().
    void socket_wake(rtlib_context_t & ctx)
    {
        std::lock_guard<std::mutex> lock(__wake_mutex);

        if (__wake_sender != __invalid)
        {
            char byte = 1;
            ::sendto(__wake_sender, &byte, 1, 0, (sockaddr *)&__wake_address, sizeof(__wake_address));
        }
    }

    // Net_Resolve(host): the first IPv4 address of the name as text, null if it is not known.
    void net_resolve(rtlib_context_t & ctx)
    {
        std::string host = __narrow(*ctx.args<__host_args_t>()->host);
        std::string result;

        __start();

        __blocking([&] {
            addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            addrinfo * list = nullptr;
            if (getaddrinfo(host.c_str(), nullptr, &hints, &list) == 0 && list != nullptr)
            {
                char text[64];
                sockaddr_in * address = (sockaddr_in *)list->ai_addr;

                if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text)) != nullptr)
                    result = text;
            }

            if (list != nullptr)
                freeaddrinfo(list);
        });

        ctx.return_<const rt_char_t *>(result.empty()? nullptr : new_rt_string(std::wstring(result.begin(), result.end())));
    }
}
