/**
    Cache Alloc Benchmark

    Copyright 2021 Phil Bouchard <phil@fornux.com>

    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt
*/    


#include "cache_alloc.hpp"

#include <iostream>
#include <chrono>
#include <list>

#define BOOST_POOL_NO_MT
#include <boost/pool/pool_alloc.hpp>


template <template <typename...> class C, typename A, size_t L>
    auto test1()
    {
        C<int, A> c;

        auto start = std::chrono::steady_clock::now();
        
        {        
            for (int i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
        }
        
       auto end = std::chrono::steady_clock::now();

       return std::chrono::duration<double>{end - start};
    }

template <template <typename...> class C, typename A, size_t L>
    auto test2()
    {
        C<int, A> c;

        auto start = std::chrono::steady_clock::now();
        
        {        
            for (int i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
            
            for (int i = 0; i < L; ++ i)
            {
                c.pop_back();
            }
        }
        
       auto end = std::chrono::steady_clock::now();

       return std::chrono::duration<double>{end - start};
    }

template <template <typename...> class C, typename A, size_t L>
    auto test3()
    {
        auto start = std::chrono::steady_clock::now();
        
        {        
            C<int, A> c;

            for (int i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
            
            for (int i = 0; i < L; ++ i)
            {
                c.pop_back();
            }
        }
        
       auto end = std::chrono::steady_clock::now();

       return std::chrono::duration<double>{end - start};
    }

template <template <typename...> class C>
    void test()
    {
        using namespace std;
        using namespace boost;
        using namespace fornux;
        
        size_t const LOOP_SIZE = 1024 * 1000;
        
        {
            cout << "cache_alloc speedup factor (emplace_back):    " << endl;
            
            auto s = test1<C, allocator<int>, LOOP_SIZE>();
        
            cout << "cache_alloc of 1 K: " << s / test1<C, cache_alloc<int, 1>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 10 K: " << s / test1<C, cache_alloc<int, 10>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 100 K: " << s / test1<C, cache_alloc<int, 100>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 1000 K: " << s / test1<C, cache_alloc<int, 1000>, LOOP_SIZE>() << "x    " << endl;
        }
    
        {
            cout << "cache_alloc speedup factor (emplace_back / pop_back):    " << endl;
            
            auto s = test2<C, allocator<int>, LOOP_SIZE>();
        
            cout << "cache_alloc of 1 K: " << s / test2<C, cache_alloc<int, 1>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 10 K: " << s / test2<C, cache_alloc<int, 10>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 100 K: " << s / test2<C, cache_alloc<int, 100>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 1000 K: " << s / test2<C, cache_alloc<int, 1000>, LOOP_SIZE>() << "x    " << endl;
        }
    
        {
            cout << "cache_alloc speedup factor (emplace_back / pop_back / destroy):    " << endl;
            
            auto s = test2<C, allocator<int>, LOOP_SIZE>();
        
            cout << "cache_alloc of 1 K: " << s / test3<C, cache_alloc<int, 1>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 10 K: " << s / test3<C, cache_alloc<int, 10>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 100 K: " << s / test3<C, cache_alloc<int, 100>, LOOP_SIZE>() << "x    " << endl;
            cout << "cache_alloc of 1000 K: " << s / test3<C, cache_alloc<int, 1000>, LOOP_SIZE>() << "x    " << endl;
        }
    
        typedef fast_pool_allocator<int, default_user_allocator_new_delete, details::pool::default_mutex, 64, 128> boost_fast_allocator;
        
        {
            cout << "boost_fast_allocator speedup factor (emplace_back):    " << endl;
            
            auto s = test1<C, allocator<int>, LOOP_SIZE>();
        
            cout << "boost_fast_allocator: " << s / test1<C, boost_fast_allocator, LOOP_SIZE>() << "x    " << endl;
        }
    
        {
            cout << "boost_fast_allocator speedup factor (emplace_back / pop_back):    " << endl;
            
            auto s = test2<C, allocator<int>, LOOP_SIZE>();
        
            cout << "boost_fast_allocator: " << s / test2<C, boost_fast_allocator, LOOP_SIZE>() << "x    " << endl;
        }

        {
            cout << "boost_fast_allocator speedup factor (emplace_back / pop_back / destroy):    " << endl;
            
            auto s = test2<C, allocator<int>, LOOP_SIZE>();
        
            cout << "boost_fast_allocator: " << s / test3<C, boost_fast_allocator, LOOP_SIZE>() << "x    " << endl;
        }
    }

int main()
{
    test<std::list>();
    
    return 0;
}

