#include <rtlib.h>

#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace X_ROOT_NS::modules::rtlib {

    ////////// ////////// ////////// ////////// //////////

    // The names and the values of the items of an enum, read from the table of reflection of the enum when they are
    // first needed. They are searched one by one when there are few of them, by hash tables when there are many.

    namespace
    {
        const size_t __hash_threshold = 8;
        const int64_t __const_flag = 1 << 10;

        typedef std::basic_string<rt_char_t> __text_t;

        static __text_t __lower(const rt_char_t * s, size_t length)
        {
            __text_t text(s, length);
            for (rt_char_t & c : text)
                c = (rt_char_t)std::towlower((wint_t)c);

            return text;
        }

        struct __item_t
        {
            const rt_char_t *   name;
            int64_t             value;
        };

        struct __items_t
        {
            std::vector<__item_t>                       items;
            bool                                        hashed = false;
            std::unordered_map<int64_t, size_t>         by_value;       // The first item of a value.
            std::unordered_map<__text_t, size_t>        by_name;
            std::unordered_map<__text_t, size_t>        by_lower_name;  // The first item of a name, ignoring the case.

            const __item_t * find_value(int64_t value) const
            {
                if (!hashed)
                {
                    for (const __item_t & item : items)
                    {
                        if (item.value == value)
                            return &item;
                    }

                    return nullptr;
                }

                auto it = by_value.find(value);
                return it == by_value.end()? nullptr : &items[it->second];
            }

            const __item_t * find_name(const rt_char_t * name, size_t length, bool ignore_case) const
            {
                if (!hashed)
                {
                    __text_t key = ignore_case? __lower(name, length) : __text_t(name, length);

                    for (const __item_t & item : items)
                    {
                        size_t item_length = (size_t)rt_string_length(item.name);
                        if (item_length != length)
                            continue;

                        __text_t item_key = ignore_case? __lower(item.name, item_length)
                                                       : __text_t(item.name, item_length);
                        if (item_key == key)
                            return &item;
                    }

                    return nullptr;
                }

                if (ignore_case)
                {
                    auto it = by_lower_name.find(__lower(name, length));
                    return it == by_lower_name.end()? nullptr : &items[it->second];
                }

                auto it = by_name.find(__text_t(name, length));
                return it == by_name.end()? nullptr : &items[it->second];
            }
        };

        // Reads the items: the constants of the own type of the enum.
        static std::unique_ptr<__items_t> __read_items(const rt_reflect_type_t * table)
        {
            std::unique_ptr<__items_t> result(new __items_t());

            const rt_reflect_list_t & fields = table->lists[0];
            for (int64_t k = 0; k < fields.count; k++)
            {
                const rt_reflect_member_t * m = fields.items[k];
                if ((m->flags & __const_flag) == 0 || m->value_kind != (int64_t)rt_reflect_value_kind_t::value
                        || m->type != m->declaring)
                    continue;

                result->items.push_back({ m->name, m->offset });
            }

            if (result->items.size() > __hash_threshold)
            {
                result->hashed = true;

                for (size_t k = 0; k < result->items.size(); k++)
                {
                    const __item_t & item = result->items[k];
                    size_t length = (size_t)rt_string_length(item.name);

                    result->by_value.emplace(item.value, k);
                    result->by_name.emplace(__text_t(item.name, length), k);
                    result->by_lower_name.emplace(__lower(item.name, length), k);
                }
            }

            return result;
        }

        static const __items_t & __items_of(const rt_reflect_type_t * table)
        {
            static std::mutex mutex;
            static std::unordered_map<const void *, std::unique_ptr<__items_t>> cache;

            std::lock_guard<std::mutex> lock(mutex);

            std::unique_ptr<__items_t> & items = cache[table];
            if (items == nullptr)
                items = __read_items(table);

            return *items;
        }

        // Parses a decimal number with a sign.
        static bool __parse_number(const rt_char_t * s, size_t length, int64_t * out)
        {
            size_t k = 0;
            bool negative = false;

            if (k < length && (s[k] == '-' || s[k] == '+'))
                negative = s[k++] == '-';

            if (k == length)
                return false;

            uint64_t value = 0;
            for (; k < length; k++)
            {
                if (s[k] < '0' || s[k] > '9')
                    return false;

                value = value * 10 + (uint64_t)(s[k] - '0');
            }

            *out = negative? -(int64_t)value : (int64_t)value;
            return true;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // The text of the value: the name of its item, the number when it has no item.
    const rt_char_t * enum_text_of(const rt_reflect_type_t * table, int64_t number)
    {
        if (const __item_t * item = __items_of(table).find_value(number))
            return item->name;

        return new_rt_string(std::to_wstring(number));
    }

    // Parses the text: the name of an item, a number, or the ones that are separated by commas (they are or-ed).
    bool enum_parse_text(const rt_reflect_type_t * table, const rt_char_t * text, bool ignore_case, int64_t * out)
    {
        const __items_t & items = __items_of(table);
        size_t length = text == nullptr? 0 : (size_t)rt_string_length(text);

        int64_t result = 0;
        size_t start = 0;

        for (;;)
        {
            size_t end = start;
            while (end < length && text[end] != ',')
                end++;

            // The spaces around a part are not its text.
            size_t b = start, e = end;
            while (b < e && std::iswspace((wint_t)text[b]))
                b++;

            while (e > b && std::iswspace((wint_t)text[e - 1]))
                e--;

            if (b == e)
                return false;

            int64_t value;
            if (const __item_t * item = items.find_name(text + b, e - b, ignore_case))
                value = item->value;
            else if (!__parse_number(text + b, e - b, &value))
                return false;

            result |= value;

            if (end >= length)
                break;

            start = end + 1;
        }

        *out = result;
        return true;
    }

}   // namespace X_ROOT_NS::modules::rtlib
