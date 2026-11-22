#include <rtlib.h>
#include <thread_manager.h>

#include <cstdio>
#include <cerrno>
#include <filesystem>
#include <iostream>
#include <map>
#include <mutex>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // The functions of System.IO: the files and the directories (the console input is here too). A call that waits for the system
    // gives the lock of the runtime back, so the other threads go on (see thread_manager_t::blocking).
    //
    // A function that can fail returns a negative code: -1 the file was not found, -2 it exists, -3 no permission, -4 any other error,
    // -5 the directory was not found. The library of the language makes the exception of it.

    namespace
    {
        namespace fs = std::filesystem;

        const int __not_found = -1, __exists = -2, __denied = -3, __failed = -4, __no_directory = -5;

        // The files that are open: the handle is a number, the library of the language keeps it.
        struct __file_t
        {
            FILE *  fp = nullptr;
        };

        std::mutex                      __files_mutex;
        std::map<int64_t, __file_t>     __files;
        int64_t                         __last_handle = 0;

        struct __path_args_t
        {
            rstring_t   path;
        };

        struct __path2_args_t
        {
            rstring_t   from;
            rstring_t   to;
        };

        struct __path2_bool_args_t
        {
            rstring_t   from;
            rstring_t   to;
            rbool_t     flag;
        };

        struct __path_bool_args_t
        {
            rstring_t   path;
            rbool_t     flag;
        };

        struct __open_args_t
        {
            rstring_t   path;
            rint32_t    mode;
            rint32_t    access;
        };

        struct __handle_args_t
        {
            rint64_t    handle;
        };

        struct __io_args_t
        {
            rint64_t    handle;
            rptr_t      buffer;
            rint32_t    count;
        };

        struct __seek_args_t
        {
            rint64_t    handle;
            rint64_t    offset;
            rint32_t    origin;
        };

        struct __length_args_t
        {
            rint64_t    handle;
            rint64_t    length;
        };

        fs::path __path_of(const rt_char_t * s)
        {
            return s == nullptr? fs::path() : fs::path(to_host_string(s, rt_string_length(s)));
        }

        // The code of the error of the system.
        int __code_of(const std::error_code & error)
        {
            if (!error)
                return 0;

            if (error == std::errc::no_such_file_or_directory)
                return __not_found;

            if (error == std::errc::file_exists)
                return __exists;

            if (error == std::errc::permission_denied || error == std::errc::operation_not_permitted
                    || error == std::errc::is_a_directory)
            {
                return __denied;
            }

            return __failed;
        }

        int __code_of_errno()
        {
            switch (errno)
            {
                case ENOENT:    return __not_found;
                case EEXIST:    return __exists;
                case EACCES:
                case EPERM:
                case EISDIR:    return __denied;
                default:        return __failed;
            }
        }

        FILE * __file_of(int64_t handle)
        {
            std::lock_guard<std::mutex> lock(__files_mutex);

            auto it = __files.find(handle);
            return it == __files.end()? nullptr : it->second.fp;
        }

        FILE * __open_file(const fs::path & path, const char * mode)
        {
        #ifdef _WIN32
            std::wstring wmode;
            for (const char * p = mode; *p != 0; p++)
                wmode += (wchar_t)*p;

            return _wfopen(path.c_str(), wmode.c_str());
        #else
            return fopen(path.string().c_str(), mode);
        #endif
        }

        int64_t __tell(FILE * fp)
        {
        #ifdef _WIN32
            return _ftelli64(fp);
        #else
            return ftello(fp);
        #endif
        }

        int __seek(FILE * fp, int64_t offset, int origin)
        {
        #ifdef _WIN32
            return _fseeki64(fp, offset, origin);
        #else
            return fseeko(fp, offset, origin);
        #endif
        }

        const rt_char_t * __string_of(const std::wstring & text)
        {
            return new_rt_string(text);
        }

        // Runs the function while the thread is not in the runtime (it can wait for the system).
        template<typename f_t> void __blocking(f_t f)
        {
            thread_manager_t::instance().blocking(f);
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // File_Open(path, mode, access): the handle, or the code of the error. The modes are those of FileMode: CreateNew 1, Create 2,
    // Open 3, OpenOrCreate 4, Truncate 5, Append 6; the access: Read 1, Write 2, ReadWrite 3.
    void file_open(rtlib_context_t & ctx)
    {
        __open_args_t * args = ctx.args<__open_args_t>();
        fs::path path = __path_of(*args->path);
        int mode = *args->mode, access = *args->access;

        int64_t result = 0;

        __blocking([&] {

            std::error_code error;
            bool exists = fs::exists(path, error);

            if (exists && fs::is_directory(path, error))
            {
                result = __denied;
                return;
            }

            const char * text = nullptr;

            switch (mode)
            {
                case 1:     // CreateNew
                    if (exists) { result = __exists; return; }
                    text = "w+b";
                    break;

                case 2:     // Create
                    text = "w+b";
                    break;

                case 3:     // Open
                    if (!exists) { result = __not_found; return; }
                    text = access == 1? "rb" : "r+b";
                    break;

                case 4:     // OpenOrCreate
                    text = exists? (access == 1? "rb" : "r+b") : "w+b";
                    break;

                case 5:     // Truncate
                    if (!exists) { result = __not_found; return; }
                    text = "w+b";
                    break;

                case 6:     // Append
                    text = "a+b";
                    break;

                default:
                    result = __failed;
                    return;
            }

            if (!exists && path.has_parent_path() && !fs::exists(path.parent_path(), error))
            {
                result = __no_directory;
                return;
            }

            FILE * fp = __open_file(path, text);
            if (fp == nullptr)
            {
                result = __code_of_errno();
                return;
            }

            if (mode == 6)
                __seek(fp, 0, SEEK_END);

            std::lock_guard<std::mutex> lock(__files_mutex);

            result = ++__last_handle;
            __files[result].fp = fp;
        });

        ctx.return_<int64_t>(result);
    }

    // File_Read(handle, buffer, count): the number of bytes that were read (0 at the end), -4 on error.
    void file_read(rtlib_context_t & ctx)
    {
        __io_args_t * args = ctx.args<__io_args_t>();
        FILE * fp = __file_of(*args->handle);
        void * buffer = *args->buffer;
        int count = *args->count;

        int result = __failed;

        if (fp != nullptr)
        {
            __blocking([&] {
                size_t n = fread(buffer, 1, (size_t)count, fp);
                result = (n == 0 && ferror(fp))? __failed : (int)n;
            });
        }

        ctx.return_<int32_t>(result);
    }

    // File_Write(handle, buffer, count): the number of bytes that were written, -4 on error.
    void file_write(rtlib_context_t & ctx)
    {
        __io_args_t * args = ctx.args<__io_args_t>();
        FILE * fp = __file_of(*args->handle);
        void * buffer = *args->buffer;
        int count = *args->count;

        int result = __failed;

        if (fp != nullptr)
        {
            __blocking([&] {
                size_t n = fwrite(buffer, 1, (size_t)count, fp);
                result = n < (size_t)count? __failed : (int)n;
            });
        }

        ctx.return_<int32_t>(result);
    }

    // File_Seek(handle, offset, origin): the new position (the origin is 0 begin, 1 current, 2 end), or -1.
    void file_seek(rtlib_context_t & ctx)
    {
        __seek_args_t * args = ctx.args<__seek_args_t>();
        FILE * fp = __file_of(*args->handle);
        int64_t offset = *args->offset;
        int origin = *args->origin == 0? SEEK_SET : *args->origin == 1? SEEK_CUR : SEEK_END;

        int64_t result = -1;

        if (fp != nullptr)
        {
            __blocking([&] {
                if (__seek(fp, offset, origin) == 0)
                    result = __tell(fp);
            });
        }

        ctx.return_<int64_t>(result);
    }

    // File_Length(handle): the length of the file, -1 on error.
    void file_length(rtlib_context_t & ctx)
    {
        FILE * fp = __file_of(*ctx.args<__handle_args_t>()->handle);
        int64_t result = -1;

        if (fp != nullptr)
        {
            __blocking([&] {
                fflush(fp);

                int64_t position = __tell(fp);
                if (__seek(fp, 0, SEEK_END) == 0)
                {
                    result = __tell(fp);
                    __seek(fp, position, SEEK_SET);
                }
            });
        }

        ctx.return_<int64_t>(result);
    }

    // File_SetLength(handle, length): 0, or -4.
    void file_set_length(rtlib_context_t & ctx)
    {
        __length_args_t * args = ctx.args<__length_args_t>();
        FILE * fp = __file_of(*args->handle);
        int64_t length = *args->length;

        int result = __failed;

        if (fp != nullptr)
        {
            __blocking([&] {
                fflush(fp);

            #ifdef _WIN32
                result = _chsize_s(_fileno(fp), length) == 0? 0 : __failed;
            #else
                result = ftruncate(fileno(fp), (off_t)length) == 0? 0 : __failed;
            #endif
            });
        }

        ctx.return_<int32_t>(result);
    }

    // File_Flush(handle): 0, or -4.
    void file_flush(rtlib_context_t & ctx)
    {
        FILE * fp = __file_of(*ctx.args<__handle_args_t>()->handle);
        int result = __failed;

        if (fp != nullptr)
            __blocking([&] { result = fflush(fp) == 0? 0 : __failed; });

        ctx.return_<int32_t>(result);
    }

    // File_Close(handle).
    void file_close(rtlib_context_t & ctx)
    {
        int64_t handle = *ctx.args<__handle_args_t>()->handle;
        FILE * fp = nullptr;

        {
            std::lock_guard<std::mutex> lock(__files_mutex);

            auto it = __files.find(handle);
            if (it != __files.end())
            {
                fp = it->second.fp;
                __files.erase(it);
            }
        }

        if (fp != nullptr)
            __blocking([&] { fclose(fp); });
    }

    ////////// ////////// ////////// ////////// //////////

    // File_Exists(path), Directory_Exists(path).
    void file_exists(rtlib_context_t & ctx)
    {
        fs::path path = __path_of(*ctx.args<__path_args_t>()->path);
        bool result = false;

        __blocking([&] {
            std::error_code error;
            result = fs::is_regular_file(path, error);
        });

        ctx.return_<bool>(result);
    }

    void directory_exists(rtlib_context_t & ctx)
    {
        fs::path path = __path_of(*ctx.args<__path_args_t>()->path);
        bool result = false;

        __blocking([&] {
            std::error_code error;
            result = fs::is_directory(path, error);
        });

        ctx.return_<bool>(result);
    }

    // File_Delete(path): 0 (also when the file does not exist), or the code.
    void file_delete(rtlib_context_t & ctx)
    {
        fs::path path = __path_of(*ctx.args<__path_args_t>()->path);
        int result = 0;

        __blocking([&] {
            std::error_code error;

            if (path.has_parent_path() && !fs::exists(path.parent_path(), error))
            {
                result = __no_directory;
                return;
            }

            fs::remove(path, error);
            result = __code_of(error);
        });

        ctx.return_<int32_t>(result);
    }

    // File_Move(from, to, overwrite), File_Copy(from, to, overwrite): 0, or the code.
    void file_move(rtlib_context_t & ctx)
    {
        __path2_bool_args_t * args = ctx.args<__path2_bool_args_t>();
        fs::path from = __path_of(*args->from), to = __path_of(*args->to);
        bool overwrite = *args->flag;
        int result = 0;

        __blocking([&] {
            std::error_code error;

            if (!fs::exists(from, error))
            {
                result = __not_found;
                return;
            }

            if (fs::exists(to, error))
            {
                if (!overwrite)
                {
                    result = __exists;
                    return;
                }

                fs::remove(to, error);
            }

            fs::rename(from, to, error);
            result = __code_of(error);
        });

        ctx.return_<int32_t>(result);
    }

    void file_copy(rtlib_context_t & ctx)
    {
        __path2_bool_args_t * args = ctx.args<__path2_bool_args_t>();
        fs::path from = __path_of(*args->from), to = __path_of(*args->to);
        bool overwrite = *args->flag;
        int result = 0;

        __blocking([&] {
            std::error_code error;

            if (!fs::exists(from, error))
            {
                result = __not_found;
                return;
            }

            fs::copy_file(from, to, overwrite? fs::copy_options::overwrite_existing : fs::copy_options::none, error);
            result = __code_of(error);
        });

        ctx.return_<int32_t>(result);
    }

    // Directory_Create(path): 0 (also when it exists), or the code. It creates the parents.
    void directory_create(rtlib_context_t & ctx)
    {
        fs::path path = __path_of(*ctx.args<__path_args_t>()->path);
        int result = 0;

        __blocking([&] {
            std::error_code error;
            fs::create_directories(path, error);
            result = __code_of(error);
        });

        ctx.return_<int32_t>(result);
    }

    // Directory_Delete(path, recursive): 0, or the code (-2: the directory is not empty).
    void directory_delete(rtlib_context_t & ctx)
    {
        __path_bool_args_t * args = ctx.args<__path_bool_args_t>();
        fs::path path = __path_of(*args->path);
        bool recursive = *args->flag;
        int result = 0;

        __blocking([&] {
            std::error_code error;

            if (!fs::is_directory(path, error))
            {
                result = __no_directory;
                return;
            }

            if (recursive)
            {
                fs::remove_all(path, error);
            }
            else if (!fs::is_empty(path, error))
            {
                result = __exists;
                return;
            }
            else
            {
                fs::remove(path, error);
            }

            result = __code_of(error);
        });

        ctx.return_<int32_t>(result);
    }

    // Directory_List(path): the entries, one by line: "d" or "f", ":", the name. Null if the directory does not exist.
    void directory_list(rtlib_context_t & ctx)
    {
        fs::path path = __path_of(*ctx.args<__path_args_t>()->path);
        std::wstring text;
        bool found = false;

        __blocking([&] {
            std::error_code error;
            if (!fs::is_directory(path, error))
                return;

            found = true;

            for (fs::directory_iterator it(path, error), end; !error && it != end; it.increment(error))
            {
                std::error_code entry_error;
                bool directory = it->is_directory(entry_error);

                text += directory? L"d:" : L"f:";
                text += it->path().filename().wstring();
                text += L"\n";
            }
        });

        ctx.return_<const rt_char_t *>(found? __string_of(text) : nullptr);
    }

    // Directory_Current(): the current directory.
    void directory_current(rtlib_context_t & ctx)
    {
        std::error_code error;
        ctx.return_<const rt_char_t *>(__string_of(fs::current_path(error).wstring()));
    }

    // Console_ReadLine(): the next line of the input (without the end of the line), null at the end of the input.
    void console_read_line(rtlib_context_t & ctx)
    {
        std::wstring line;
        bool read = false;

        __blocking([&] { read = (bool)std::getline(std::wcin, line); });

        if (read && !line.empty() && line.back() == L'\r')
            line.pop_back();

        ctx.return_<const rt_char_t *>(read? __string_of(line) : nullptr);
    }
}
