#ifndef CACHE_ALLOC_HPP
#define CACHE_ALLOC_HPP

/**
    Cache Alloc

    Copyright 2021 Phil Bouchard <phil@fornux.com>

    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt
*/


#include <array>
#include <list>
#include "intrusive_list.hpp"


template <typename T, size_t S, template <typename...> class A = std::allocator> // type, cache size based on speed of pre-made benchmark and allocator
    struct cache_alloc
    {
        template <class, size_t, template <typename...> class> friend struct cache_alloc;
        
        typedef T value_type;
        typedef T & reference;
        typedef T const & const_reference;
        typedef size_t size_type;
        
        template <class U> 
            struct rebind 
            {
                typedef cache_alloc<U, S> other;
            };
            
        T * allocate(size_t size) noexcept __attribute__((always_inline))
        {
            typename std::list<cache_t, A<cache_t>>::iterator pcache = -- pool.caches.end();
            
            if (! pool.elements.empty())
            {
                // reuse node
                boost::smart_ptr::detail::intrusive_list_node * const i = pool.elements.begin();
                element_t * const p = boost::smart_ptr::detail::classof(& element_t::node, i);
                    
                i->erase();
                p->pcache->size += 1;
                    
                return reinterpret_cast<T *>(& p->element);
            }
            
            if (pcache->size >= S * 1024)
            {
                // add a buffer
                pool.caches.emplace_back();
                
                pcache = -- pool.caches.end();
            }
            
            element_t * const p = & pcache->data[pcache->size];
            p->pcache = pcache;
            pcache->size += size;
            
            return reinterpret_cast<T *>(& p->element);
        }
        
        void deallocate(T * q, size_t size) noexcept __attribute__((always_inline))
        {
            element_t * const p = boost::smart_ptr::detail::classof(& element_t::element, reinterpret_cast<typename std::aligned_storage<sizeof(T), alignof(T)>::type *>(q));
            
            p->pcache->size -= size;

            // enlist this node for eventual reuse
            pool.elements.push_back(& p->node);
            
            if (pool.caches.size() > 1 && p->pcache->size == 0)
                // remove a buffer
                pool.caches.erase(p->pcache);
        }

    private:
        struct cache_t;
        
        struct element_t
        {
            typename std::list<cache_t, A<cache_t>>::iterator pcache;
            boost::smart_ptr::detail::intrusive_list_node node;
            typename std::aligned_storage<sizeof(T), alignof(T)>::type element;
        };
        
        typedef std::array<element_t, S * 1024> data_t;
        
        struct cache_t
        {
            size_t size{};
            data_t data;
        };
        
        struct pool_t
        {
            boost::smart_ptr::detail::intrusive_list elements{};
            std::list<cache_t, A<cache_t>> caches{1};
        };
        
        pool_t pool; // general pool
    };


#endif
