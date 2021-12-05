
#include <common.h>
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <algorithm.h>

namespace X_ROOT_NS { namespace os {

    ////////// ////////// ////////// ////////// //////////
    // Memory.

    // Allocates memory of specified size.
    void * malloc(size_t size)
    {
        return ::malloc(size);
    }

    // Allocates memory of specified element size and count.
    void * calloc(size_t n, size_t size)
    {
        return ::calloc(n, size);
    }

    // Reallocates memory of specified size.
    void * realloc(void * p, size_t size)
    {
        return ::realloc(p, size);
    }

    // Frees memory.
    void free(void * p)
    {
        if (p)
        {
            ::free(p);
        }
    }

    // Exit program with specified code.
    void exit(int code)
    {
        ::exit(code);
    }

    ////////// ////////// ////////// ////////// //////////
    // Console.

    template<size_t _n>
    void __append_text(char_t * & p, const char_t (&s)[_n])
    {
        al::quick_copy<_n * sizeof(char_t)>(p, s);
        p += _n - 1;
    }

    X_ENUM_INFO(console_color_t)

        X_C(none,           _T("none"))

        X_C(black,          _T("black"))

        X_C(red,            _T("red"))

        X_C(green,          _T("green"))

        X_C(brown,          _T("brown"))

        X_C(blue,           _T("blue"))

        X_C(magenta,        _T("magenta"))

        X_C(cyan,           _T("cyan"))

        X_C(gray,           _T("gray"))

        X_C(dark_gray,      _T("dark_gray"))

        X_C(light_red,      _T("light_red"))

        X_C(light_green,    _T("light_green"))

        X_C(yellow,         _T("yellow"))

        X_C(light_blue,     _T("light_blue"))

        X_C(light_magenta,  _T("light_magenta"))

        X_C(light_cyan,     _T("light_cyan"))

        X_C(white,          _T("white"))

        X_C(reset,          _T("reset"))

    X_ENUM_INFO_END

    // Set style of console window.
    void set_console_style(console_color_t color, console_color_t bg_color, xbool_t underline)
    {
        #define __WriteColor(_color, _text)                                             \
            case console_color_t::_color:                                               \
                __append_text(p, _text);                                                \
                break;

        char_t s[32] = _T("\0"), * p = s;
        __append_text(p, _T("\033["));

        // Text color.
        switch (color)
        {
            __WriteColor(black,         _T("22;30;"))
            __WriteColor(red,           _T("22;31;"))
            __WriteColor(green,         _T("22;32;"))
            __WriteColor(brown,         _T("22;33;"))
            __WriteColor(blue,          _T("22;34;"))
            __WriteColor(magenta,       _T("22;35;"))
            __WriteColor(cyan,          _T("22;36;"))
            __WriteColor(gray,          _T("22;37;"))
            __WriteColor(dark_gray,     _T("01;30;"))
            __WriteColor(light_red,     _T("01;31;"))
            __WriteColor(light_green,   _T("01;32;"))
            __WriteColor(yellow,        _T("01;33;"))
            __WriteColor(light_blue,    _T("01;34;"))
            __WriteColor(light_magenta, _T("01;35;"))
            __WriteColor(light_cyan,    _T("01;36;"))
            __WriteColor(white,         _T("01;37;"))
            __WriteColor(reset,         _T("0;"))

            default: break;
        }

        // Background color.
        switch (bg_color)
        {
            __WriteColor(black,         _T("22;40;"))
            __WriteColor(red,           _T("22;41;"))
            __WriteColor(green,         _T("22;42;"))
            __WriteColor(brown,         _T("22;43;"))
            __WriteColor(blue,          _T("22;44;"))
            __WriteColor(magenta,       _T("22;45;"))
            __WriteColor(cyan,          _T("22;46;"))
            __WriteColor(gray,          _T("22;47;"))
            __WriteColor(dark_gray,     _T("01;40;"))
            __WriteColor(light_red,     _T("01;41;"))
            __WriteColor(light_green,   _T("01;42;"))
            __WriteColor(yellow,        _T("01;43;"))
            __WriteColor(light_blue,    _T("01;44;"))
            __WriteColor(light_magenta, _T("01;45;"))
            __WriteColor(light_cyan,    _T("01;46;"))
            __WriteColor(white,         _T("01;47;"))
            __WriteColor(reset,         _T("49;"))

            default: break;
        }

        // Underline
        if (underline == xbool_t::yes)
            __append_text(p, _T("4;"));
        else if (underline == xbool_t::no)
            __append_text(p, _T("24;"));

        if (*(p - 1) == _T(';'))
        {
            *(p - 1) = _T('m');
            std::wcout << s;
        }
    }

    // Reset style of console window.
    void reset_console_style()
    {
        std::wcout << _T("\033[0m");
    }

    ////////// ////////// ////////// ////////// //////////

} }  // namespace X_ROOT_NS::os

