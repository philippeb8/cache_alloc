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
#include <stack>
#include <queue>


using namespace std;


template <template <typename...> class C, size_t L>
    auto test_new_delete1()
    {
        C<int, allocator<int>> c;

        auto start = chrono::steady_clock::now();
        
        {        
            for (size_t i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
        }
        
       auto end = chrono::steady_clock::now();

       return chrono::duration<double>{end - start};
    }

template <template <typename...> class C, size_t S, size_t L>
    auto test_cache_alloc1()
    {
        C<int, cache_alloc<int, S>> c;
            
        auto start = chrono::steady_clock::now();
        
        {
            for (size_t i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
        }
        
        auto end = chrono::steady_clock::now();

        return chrono::duration<double>{end - start};
    }

template <template <typename...> class C, size_t L>
    auto test_new_delete2()
    {
        C<int, allocator<int>> c;

        auto start = chrono::steady_clock::now();
        
        {        
            for (size_t i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
            
            for (size_t i = 0; i < L; ++ i)
            {
                c.pop_back();
            }
        }
        
       auto end = chrono::steady_clock::now();

       return chrono::duration<double>{end - start};
    }

template <template <typename...> class C, size_t S, size_t L>
    auto test_cache_alloc2()
    {
        C<int, cache_alloc<int, S>> c;
            
        auto start = chrono::steady_clock::now();
        
        {
            for (size_t i = 0; i < L; ++ i)
            {
                c.emplace_back(i);
            }
            
            for (size_t i = 0; i < L; ++ i)
            {
                c.pop_back();
            }
        }
        
        auto end = chrono::steady_clock::now();

        return chrono::duration<double>{end - start};
    }

int main()
{
    size_t const LOOP_SIZE = 1024 * 100;
        
    {
        cout << "emplace_back:    " << endl;
        
        auto s = test_new_delete1<list, LOOP_SIZE>();
    
        cout << "cache_alloc of 1 K: " << s / test_cache_alloc1<list, 1, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 10 K: " << s / test_cache_alloc1<list, 10, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 100 K: " << s / test_cache_alloc1<list, 100, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 1000 K: " << s / test_cache_alloc1<list, 1000, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 10000 K: " << s / test_cache_alloc1<list, 10000, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 100000 K: " << s / test_cache_alloc1<list, 100000, LOOP_SIZE>() << "x    " << endl;
    }

    {
        cout << "emplace_back / pop_back:    " << endl;
        
        auto s = test_new_delete2<list, LOOP_SIZE>();
    
        cout << "cache_alloc of 1 K: " << s / test_cache_alloc2<list, 1, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 10 K: " << s / test_cache_alloc2<list, 10, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 100 K: " << s / test_cache_alloc2<list, 100, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 1000 K: " << s / test_cache_alloc2<list, 1000, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 10000 K: " << s / test_cache_alloc2<list, 10000, LOOP_SIZE>() << "x    " << endl;
        cout << "cache_alloc of 100000 K: " << s / test_cache_alloc2<list, 100000, LOOP_SIZE>() << "x    " << endl;
    }
            
    return 0;
}

