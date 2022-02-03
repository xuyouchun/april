
#ifndef __ALGORITHM_PROPERTY_H__
#define __ALGORITHM_PROPERTY_H__

namespace X_ROOT_NS::algorithm {

    template<typename _property_t> struct property_enumerator_t;

    // Property management for a set of objects / values.

    template<typename _property_t, size_t _size>
    struct properties_t
    {
    public:
        typedef _property_t property_t;
        typedef typename _property_t::object_t  object_t;

        static const size_t size = _size;

        // Constructor
        properties_t()
        {
            property_enumerator_t<property_t> enumerator(__properties);
            property_t::init(enumerator);
        }

        // Get property of specified object.
        X_INLINE property_t & get_property(const object_t & obj)
        {
            return __properties[property_t::to_index(obj)];
        }

        // Get property of specified object.
        X_INLINE const property_t & get_property(const object_t & obj) const
        {
            return __properties[property_t::to_index(obj)];
        }

        // Get property of specified object.
        X_INLINE property_t & operator[](const object_t & obj)
        {
            return get_property(obj);
        }

        // Get property of specified object.
        X_INLINE const property_t & operator[](const object_t & obj) const
        {
            return get_property(obj);
        }

        // Returns if a object is over limit.
        X_INLINE static bool overlimit(const object_t & obj)
        {
            return property_t::to_index(obj) >= _size;
        }

    private:
        property_t __properties[_size];
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of object property.
    template<typename _property_t, typename _object_t>
    struct property_t
    {
        typedef _object_t object_t;
        typedef property_enumerator_t<_property_t> enumerator_t;
        template<size_t size=(size_t)max_value<_object_t>()>
            using properties_t = properties_t<_property_t, size>;

        // Do initialize
        static void init(enumerator_t & e) { }

        // Converts object to index.
        static size_t to_index(const object_t & p)
        {
            return (size_t)p;
        }

        // Gets the object at the index.
        static object_t from_index(size_t index)
        {
            return (object_t)index;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Base class of object property with flags.
    template<typename _property_t, typename _object_t, typename _flag_t=int32_t>
    struct flag_property_t : public property_t<_property_t, _object_t>
    {
    public:
        typedef property_enumerator_t<_property_t> enumerator_t;
        typedef _flag_t flag_t;

        // Constructor
        flag_property_t() : __flag(_flag_t()) { }

        // Sets flag
        void set_flag(flag_t flag)
        {
            *(__flag_int_t *)&__flag |= (__flag_int_t)flag;
        }

        // Clears flag
        void clear_flag(flag_t flag)
        {
            *(__flag_int_t *)&__flag &= ~(__flag_int_t)flag;
        }

        // Returns whether the property has specified flag.
        bool has_flag(flag_t flag) const
        {
            return *(__flag_int_t *)&__flag & (__flag_int_t)flag;
        }

        // Batch set flags.
        static void set_flag(enumerator_t & e,
            const _object_t & start, const _object_t & end, flag_t flag)
        {
            e.each(start, end, [flag](const _object_t & obj, _property_t & p) {
                p.set_flag(flag);
            });
        }

        // Batch set flags.
        template<size_t n>
        static void set_flag(enumerator_t & e, const _object_t (&objs)[n], flag_t flag)
        {
            e.each(objs, [flag](const _object_t & obj, _property_t & p) {
                p.set_flag(flag);
            });
        }

        // Batch set flags.
        static void set_flag(enumerator_t & e, const _object_t & obj, flag_t flag)
        {
            e.each([flag](const _object_t & obj, _property_t & p) {
                p.set_flag(flag);
            }, obj);
        }

        // Batch set flags.
        template<typename ... args_t>
        static void set_flag(enumerator_t & e, flag_t flag, args_t ... args)
        {
            e.each([flag](const _object_t & obj, _property_t & p) {
                p.set_flag(flag);
            }, args ...);
        }

    private:
        typedef int_type_t<sizeof(flag_t), false> __flag_int_t;
        flag_t __flag;
    };

    //-------- ---------- ---------- ---------- ----------

    // Property enumerator for walking all properties.

    template<typename _property_t>
    struct property_enumerator_t
    {
    public:
        typedef _property_t property_t;
        typedef typename _property_t::object_t object_t;

        // Constructor.
        template<size_t size>
        property_enumerator_t(property_t (&properties)[size])
            : __properties(properties), __size(size) { }

        // Walks all properties.
        template<typename func_t>
        void each(func_t func)
        {
            for (size_t index = 0; index < __size; index++)
            {
                func(property_t::from_index(index), __properties[index]);
            }
        }

        // Walks specified properties.
        template<typename func_t, typename ... objs_t>
        void each(func_t func, const object_t & obj, objs_t ... objs)
        {
            each(func, obj);
            each(func, objs ...);
        }

        // Walks specified property.
        template<typename func_t>
        void each(func_t func, const object_t & obj)
        {
            func(obj, __properties[property_t::to_index(obj)]);
        }

        // Walks specified properties.
        template<typename func_t>
        void each(const object_t & start, const object_t & end, func_t func)
        {
            size_t start_index = property_t::to_index(start);
            size_t end_index = property_t::to_index(end);

            for (size_t index = start_index; index <= end_index; index++)
            {
                func(property_t::from_index(index), __properties[index]);
            }
        }

        // Walks specified properties.
        template<typename func_t, size_t n>
        void each(const object_t (&objs)[n], func_t func)
        {
            for (int k = 0; k < n; k++)
            {
                const object_t & obj = objs[k];
                func(obj, __properties[property_t::to_index(obj)]);
            }
        }

        // Gets property for a object.
        property_t & operator [](const object_t & obj)
        {
            return __properties[property_t::to_index(obj)];
        }

    private:
        property_t * const __properties;
        const size_t __size;
    };

    ////////// ////////// ////////// ////////// //////////

    /*

    // Examples:

    enum class my_flag_t : uint8_t
    {
        is_whitespace = (1 << 0),

        is_word       = (1 << 1),
    };

    struct my_property_t : public flag_property_t<my_property_t, char>
    {
        static void init(enumerator_t & e)
        {
            set_flag(e, my_flag_t::is_whitespace, ' ', '\t', '\r', '\n', '\0');
        }
    };

    static my_propert_t::properties_t<127> __properties;

    bool is_whitespace(char_t c)
    {
        return __properties.overlimit(c)? false
            : __properties[c].has_flag(my_flag_t::is_whitespace);
    }

    bool is_word(char_t c)
    {
        return __properties.overlimit(c)? false
            : __properties[c].has_flag(my_flag_t::is_word);
    }

    */

    ////////// ////////// ////////// ////////// //////////

}  // X_ROOT_NS::algorithm

#endif  // __ALGORITHM_PROPERTY_H__
