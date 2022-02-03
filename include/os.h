
#ifndef __OS_H__
#define __OS_H__

#include <common.h>

namespace X_ROOT_NS::os {

    ////////// ////////// ////////// ////////// //////////
    // Memory.

    // Allocates memory of specified size.
    void * malloc(size_t size);

    // Allocates memory of specified element size and count.
    void * calloc(size_t n, size_t size);

    // Reallocates memory of specified size.
    void * realloc(void * p, size_t size);

    // Frees memory.
    void free(void * p);

    // Exit program with specified code.
    void exit(int code);

    ////////// ////////// ////////// ////////// //////////
    // Console.

    X_ENUM(console_color_t)

        none = __default__,

        black,

        red,

        green,

        brown,

        blue,

        magenta,

        cyan,

        gray,

        dark_gray,

        light_red,

        light_green,

        yellow,

        light_blue,

        light_magenta,

        light_cyan,

        white,

        reset = 0x7E,

    X_ENUM_END

    // Set style of console window.
    void set_console_style(console_color_t color,
        console_color_t bg_color = console_color_t::none,
        xbool_t underline = xbool_t::none);

    // Reset style of console window.
    void reset_console_style();

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::os


#endif // __OS_H__

