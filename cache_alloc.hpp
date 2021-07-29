/**
    Cache Alloc

    Copyright 2021 Phil Bouchard <phil@fornux.com>

    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef CACHE_ALLOC_HPP
#define CACHE_ALLOC_HPP

#include <array>
#include "list.hpp"

#if BOOST_BENCHMARK
#include <iostream>

#ifdef _WIN32
#include <intrin.h>

inline uint64_t rdtsc()
{
    return __rdtsc();
}
#else
inline uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif
#endif


namespace fornux
{


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
                typedef cache_alloc<U, S, A> other;
            };
            
        T * allocate(size_t size) noexcept __attribute__((always_inline))
        {
            typename fornux::list<cache_t, A<cache_t>>::iterator pcache;

            if (pool.dead_caches.empty())
            {
#ifdef BOOST_BENCHMARK
                benchmark_t cache(stats.cache);
                
#endif
                // add a buffer
                pool.caches.emplace_back();
                
                pcache = pool.caches.rbegin();
                
                pool.dead_caches.push_back(& pcache->pool_node);
            }
            else
            {
                pcache = boost::smart_ptr::detail::classof(& cache_t::pool_node, pool.dead_caches.rbegin());
            }
                        
            if (pcache->live_elements_size < S * 1024)
            {
#ifdef BOOST_BENCHMARK
                benchmark_t element(stats.element);
                
#endif
                // create new element
                element_t * const pelement = & reinterpret_cast<data_t &>(pcache->data)[pcache->live_elements_size];
                
                pelement->pcache = pcache;
                pelement->pcache->live_elements_size += size;
                new (& pelement->cache_node) boost::smart_ptr::detail::intrusive_list_node();
                
                if (pcache->live_elements_size >= S * 1024)
                    pcache->pool_node.erase();
                
                return reinterpret_cast<T *>(& pelement->element);
            }
            else
            {
#ifdef BOOST_BENCHMARK
                benchmark_t element(stats.element);
                
#endif
                // reuse element
                element_t * const pelement = boost::smart_ptr::detail::classof(& element_t::cache_node, pcache->dead_elements.begin());
                    
                pelement->cache_node.erase();
                
                if (pcache->dead_elements.empty())
                    pcache->pool_node.erase();
                    
                return reinterpret_cast<T *>(& pelement->element);
            }
        }
        
        void deallocate(T * q, size_t size) noexcept __attribute__((always_inline))
        {
            element_t * const pelement = boost::smart_ptr::detail::classof(& element_t::element, reinterpret_cast<typename std::aligned_storage<sizeof(T), alignof(T)>::type *>(q));
            
            {
#ifdef BOOST_BENCHMARK
                benchmark_t element(stats.element);
                
#endif
                // enlist this pool_node for eventual reuse
                pelement->pcache->live_elements_size -= size;
                pelement->pcache->dead_elements.push_back(& pelement->cache_node);
                pelement->pcache->pool_node.erase();
                pool.dead_caches.push_back(& pelement->pcache->pool_node);
            }
            
            if (pool.caches.size() > 1 && pelement->pcache->live_elements_size == 0)
            {
#ifdef BOOST_BENCHMARK
                benchmark_t cache(stats.cache);
                
#endif
                // remove a buffer                
                pool.caches.erase(pelement->pcache);
            }
        }
#ifdef BOOST_BENCHMARK

        ~cache_alloc()
        {
            if (double(stats.element.time + stats.cache.time) / double(stats.element.time) > 1.0)
                std::cerr << "(buffer non-optimal) ";
        }
#endif

    private:
#ifdef BOOST_BENCHMARK
        struct stats_t
        {
            struct unit_t
            {
                //size_t count{};
                uint64_t time{};
            } element, cache;
        } stats;

        struct benchmark_t
        {
            uint64_t start;
            typename stats_t::unit_t & unit;
            
            benchmark_t(typename stats_t::unit_t & unit) 
            : start(rdtsc())
            , unit(unit)
            {
                //++ unit.count;
            }
            
            ~benchmark_t()
            {
                uint64_t end = rdtsc();
                
                unit.time += end - start;
            }
        };
        
#endif
        struct cache_t;
        
        struct element_t
        {
            boost::smart_ptr::detail::intrusive_list_node cache_node;
            typename fornux::list<cache_t, A<cache_t>>::iterator pcache;
            typename std::aligned_storage<sizeof(T), alignof(T)>::type element;
        };
        
        typedef std::array<element_t, S * 1024> data_t;
        
        struct cache_t
        {
            size_t live_elements_size{};
            boost::smart_ptr::detail::intrusive_list_node pool_node;
            boost::smart_ptr::detail::intrusive_list dead_elements;
            typename std::aligned_storage<sizeof(data_t), alignof(data_t)>::type data;
        };
        
        struct pool_t
        {
            boost::smart_ptr::detail::intrusive_list dead_caches;
            fornux::list<cache_t, A<cache_t>> caches{1};
            
            pool_t()
            {
                dead_caches.push_back(& caches.begin()->pool_node);
            }
        };
        
        pool_t pool; // general pool
    };

}


#endif
