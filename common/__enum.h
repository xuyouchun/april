
#ifndef __COMMON_ENUM_H__
#define __COMMON_ENUM_H__

namespace X_ROOT_NS {

    ////////// ////////// ////////// ////////// //////////

    // An enum info structure that contains name, title, desc of a enum value.

    template<typename enum_t>
    struct enum_info_t
    {
        enum_t         value;
        const char_t * name;
        const char_t * title;
        const char_t * desc;

        operator enum_t() const { return value; }
        operator   bool() const { return value != enum_t::__unknown__; }
    };

    //-------- ---------- ---------- ---------- ----------

    // 

    template<typename enum_t>
    struct __enum_init_item_t
    {
        __enum_init_item_t(enum_t value, const char_t * name, const char_t * title = nullptr,
                                                                const char_t * desc = nullptr)
            : value(value), name(name), title(title? title : name), desc(desc? desc : _T(""))
        { }

        const enum_t value;
        const char_t * name, * title, * desc;
    };

    //-------- ---------- ---------- ---------- ----------

    // An enum info map helping search an enum value by title or name.

    template<typename enum_t>
    struct __enum_info_map
    {
        std::map<enum_t,   const enum_info_t<enum_t> *> enum_map;
        std::map<string_t, const enum_info_t<enum_t> *> name_map;
        std::map<string_t, const enum_info_t<enum_t> *> title_map;
    };

    namespace
    {
        // Init enum info map by give arguments.

        template<typename enum_t>
        void __init_enum_info_map(std::map<enum_t, const enum_info_t<enum_t> *> & map,
            enum_info_t<enum_t> * buffer, __enum_init_item_t<enum_t> & item)
        {
            // the last item, do nothing
        }

        template<typename enum_t, typename ... args_t>
        void __init_enum_info_map(std::map<enum_t, const enum_info_t<enum_t> *> & map,
            enum_info_t<enum_t> * buffer, __enum_init_item_t<enum_t> & item, args_t ... args)
        {
            buffer->value = item.value;
            buffer->name  = item.name;
            buffer->title = item.title;
            buffer->desc  = item.desc;

            map[item.value] = buffer;
            __init_enum_info_map(map, buffer + 1, args...);
        }
    }

    //-------- ---------- ---------- ---------- ----------

    // Init enum info map by give arguments.

    template<typename enum_t, typename ... args_t>
    const __enum_info_map<enum_t> * __init_enum_info_map(args_t ... args)
    {
        __enum_info_map<enum_t> * map = new __enum_info_map<enum_t>();
        enum_info_t<enum_t> * buffer = new enum_info_t<enum_t>[(sizeof...(args))];
        __init_enum_info_map<enum_t>(map->enum_map, buffer, args ...);

        for (auto it = map->enum_map.begin(), it_end = map->enum_map.end(); it != it_end; it++)
        {
            const enum_info_t<enum_t> * info = it->second;
            map->name_map[string_t(info->name)] = info;
            map->title_map[string_t(info->title)] = info;
        }

        return map;
    }

    //-------- ---------- ---------- ---------- ----------

    enum class enum_search_type_t { name, title, all };

    // A set of enum tools.
    template<typename enum_t> struct __enum_op_t
    {
        // Searchs enum info by title/name.
        static const enum_info_t<enum_t> & search(const char_t * s, enum_search_type_t search_type)
        {
            if (!s)
                return _enum(enum_t::__unknown__);
            return search(string_t(s), search_type);
        }

        // Searchs enum info by title/name.
        static const enum_info_t<enum_t> & search(const string_t & s, enum_search_type_t search_type)
        {
            const enum_info_t<enum_t> * enum_info;
            auto map = __get_enum_info_map(enum_t());
            switch (search_type)
            {
                case enum_search_type_t::name:
                    enum_info = __try_search(map.name_map, s);
                    break;

                case enum_search_type_t::title:
                    enum_info = __try_search(map.title_map, s);
                    break;

                default:
                    enum_info = __try_search(map.name_map, s);
                    if (!enum_info)
                        enum_info = __try_search(map.title_map, s);
                    break;
            }

            return enum_info? *enum_info : _enum(enum_t::__unknown__);
        }

        // Tries to search a enum info by title/name, return nullptr when not found.
        static const enum_info_t<enum_t> * __try_search(
                std::map<string_t, const enum_info_t<enum_t> *> & map, const string_t & s)
        {
            auto it = map.find(s);
            return it == map.cend()? nullptr : it->second;
        }
    };

    //-------- ---------- ---------- ---------- ----------
    // Macro definations of enum type. a set of functions.

    #define X_ENUM_INFO_DECLARE(enum_t)                                                 \
        const enum_info_t<enum_t> & _enum(enum_t value);                                \
        const enum_info_t<enum_t> & _enum(enum_t unknown, const string_t & s,           \
                enum_search_type_t search_type=enum_search_type_t::name);               \
        const enum_info_t<enum_t> & _enum(enum_t unknown, const char_t * s,             \
                enum_search_type_t search_type=enum_search_type_t::name);               \
        const char_t * _title(enum_t value);                                            \
        const string_t _stitle(enum_t value);                                           \
        const char_t * _name(enum_t value);                                             \
        const string_t _sname(enum_t value);                                            \
        const char_t * _desc(enum_t value);                                             \
        const string_t _sdesc(enum_t value);                                            \
        const __enum_info_map<enum_t> & __get_enum_info_map(enum_t);                    \
                                                                                        \
        template<typename callback_t>                                                   \
        void each(enum_t value, callback_t callback)                                    \
        {                                                                               \
            auto map = __get_enum_info_map(enum_t()).enum_map;                          \
            for (auto it = map.cbegin(); it != map.cend(); it++)                         \
            {                                                                           \
                callback(*it->second);                                                  \
            }                                                                           \
        }

    #define __X_C__(name, args...)                                                      \
            ::X_ROOT_NS::__enum_init_item_t<t>(t::name, _S(name), ##args)
    #define X_C(name, args...) __X_C__(name, ##args),
    #define X_D(name, desc)    __X_C__(name, _S(name), desc),

    // Defines title/name/description for enum types.
    #define X_ENUM_INFO(enum_t)                                                         \
                                                                                        \
        const enum_info_t<enum_t> & _enum(enum_t value)                                 \
        {                                                                               \
            auto map = __get_enum_info_map(enum_t()).enum_map;                          \
            auto it = map.find(value);                                                  \
            return it == map.cend()? _enum(enum_t::__unknown__) : *it->second;          \
        }                                                                               \
                                                                                        \
        const enum_info_t<enum_t> & _enum(enum_t nouse, const string_t & s,             \
                enum_search_type_t search_type)                                         \
        {                                                                               \
            return __enum_op_t<enum_t>::search(s, search_type);                         \
        }                                                                               \
                                                                                        \
        const enum_info_t<enum_t> & _enum(enum_t nouse, const char_t * s,               \
                enum_search_type_t search_type)                                         \
        {                                                                               \
            return __enum_op_t<enum_t>::search(s, search_type);                         \
        }                                                                               \
                                                                                        \
        const char_t * _title(enum_t value)                                             \
        {                                                                               \
            return _enum(value).title;                                                  \
        }                                                                               \
                                                                                        \
        const string_t _stitle(enum_t value)                                            \
        {                                                                               \
            typedef int_type_t<sizeof(value)> underlying_t;                             \
            const enum_info_t<enum_t> & info = _enum(value);                            \
            return info.value != value? sprintf(_T("[") _S(enum_t) _T(":%1%]"),         \
                    (underlying_t)value) : string_t(info.title);                        \
        }                                                                               \
                                                                                        \
        const char_t * _name(enum_t value)                                              \
        {                                                                               \
            return _enum(value).name;                                                   \
        }                                                                               \
                                                                                        \
        const string_t _sname(enum_t value)                                             \
        {                                                                               \
            typedef int_type_t<sizeof(value)> underlying_t;                             \
            const enum_info_t<enum_t> & info = _enum(value);                            \
            return info.value != value? sprintf(_T("[") _S(enum_t) _T(":%1%]"),         \
                    (underlying_t)value) : string_t(info.name);                         \
        }                                                                               \
                                                                                        \
        const char_t * _desc(enum_t value)                                              \
        {                                                                               \
            return _enum(value).desc;                                                   \
        }                                                                               \
                                                                                        \
        const string_t _sdesc(enum_t value)                                             \
        {                                                                               \
            typedef int_type_t<sizeof(value)> underlying_t;                             \
            const enum_info_t<enum_t> & info = _enum(value);                            \
            return info.value != value? sprintf(_T("[") _S(enum_t) _T(":%1%]"),         \
                    (underlying_t)value) : string_t(info.desc);                         \
        }                                                                               \
                                                                                        \
        const __enum_info_map<enum_t> & __get_enum_info_map(enum_t v)                   \
        {                                                                               \
            typedef enum_t t;                                                           \
            static const __enum_info_map<enum_t> * map = __init_enum_info_map<t>(       \
                X_C(__unknown__)                                                        \
                X_C(__default__)                                                        \

    // The end of X_ENUM_INFO
    #define X_ENUM_INFO_END                                                             \
            __X_C__(__end__) );                                                         \
            return *map;                                                                \
        }
 
    #define X_ENUM_(enum_t, size)                                                       \
        enum class enum_t : int_type_t<size>;                                           \
        X_ENUM_INFO_DECLARE(enum_t)                                                     \
        enum class enum_t : int_type_t<size>                                            \
        {                                                                               \
            __unknown__ = -1,                                                           \
            __default__ = 0,                                                            \

    // The end of X_ENUM
    #define X_ENUM_END                                                                  \
            __end__,                                                                    \
        };

    // Defines a enum type.
    #define X_ENUM(enum_t)  X_ENUM_(enum_t, 1)

    #define X_EN(type, name_)  \
        _enum(type::__unknown__, name_, ::X_ROOT_NS::enum_search_type_t::name)

    #define X_ET(type, title_) \
        _enum(type::__unknown__, title_, ::X_ROOT_NS::enum_search_type_t::title)


    //-------- ---------- ---------- ---------- ----------

    /*
        Example: 

        // some_file.h

        X_ENUM(enum_t)
            argument_error,
            format_error,
        X_ENUM_END


        // some_file.cpp

        X_ENUM_INFO(enum_t)
            X_C(argument_error, _T("argument error"))
            X_C(format_error,   _T("format error"))
        X_ENUM_INFO_END()
    */

}

#endif  // __COMMON_ENUM_H__
