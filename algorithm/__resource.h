
#ifndef __ALGORITHM_RESOURCE_H__
#define __ALGORITHM_RESOURCE_H__


namespace X_ROOT_NS { namespace algorithm {

    ////////// ////////// ////////// ////////// //////////

    // Resource pool for keep release objects into pool, then used for next creating.

    template<typename resource_object_t>
    class resource_pool_t : public object_t
    {
        typedef uint_type_t<sizeof(resource_object_t *)> __key_t;

    public:
        // Constructor, with specified memory managerment and creator.
        resource_pool_t(memory_t * memory, creator_t * creator,
                    size_t capacity=std::max<size_t>::value)
            : __memory(memory), __creator(creator), __capacity(capacity)
        {
            _A(memory != nullptr);
            _A(creator != nullptr);
        }

        // Acquires a object, Creates when the pool is empty.
        resource_object_t * acquire()
        {
            mutex_lock_t _1(__mutex);
            return __acquire(); 
        }

        // Releases a object, Then put it into the pool.
        void release(resource_object_t * obj)
        {
            _A(obj != nullptr);

            mutex_lock_t _1(__mutex);
            return __release();
        }

        // Destructor, Releases all objects in the pool.
        virtual ~resource_pool_t() override
        {
            mutex_lock_t _1(__mutex);
            __dealloc();
        }

    private:
        std::set<resource_object_t *> __set;
        memory_t * __memory;
        creator_t * __creator;
        size_t __capacity;
        volatile size_t __count = 0;
        std::mutex __mutex;

        // Acquires a object, Creates when the pool is empty.
        resource_object_t * __acquire()
        {
            if(__set.size() == 0)
            {
                if(__count >= __capacity)
                    
            }
            else
            {
                auto it = __set.begin();
                resource_object_t * obj = *it;
                __set.erase(it);

                return obj;
            }
        }

        // Releases a object, then put it into the pool.
        void __release(resource_object_t * obj)
        {
            _A(obj != nullptr);

            __set.add(obj);
        }

        void __dealloc()
        {
            
        }
    };

    ////////// ////////// ////////// ////////// //////////


} } // X_ROOT_NS::algorithm


#endif // __ALGORITHM_RESOURCE_H__
