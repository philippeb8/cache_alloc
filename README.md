# cache_alloc
Cached buffer allocator based on types, and usage frequency.

emplace_back:    
cache_alloc of 1 K: 2.34402x    
cache_alloc of 10 K: 1.57723x    
cache_alloc of 100 K: 7.32788x    
cache_alloc of 1000 K: 6.79913x    
cache_alloc of 10000 K: 6.62675x    
cache_alloc of 100000 K: 6.49273x    
emplace_back / pop_back:    
cache_alloc of 1 K: 1.57572x    
cache_alloc of 10 K: 1.91624x    
cache_alloc of 100 K: 2.8606x    
cache_alloc of 1000 K: 2.73628x    
cache_alloc of 10000 K: 2.6896x    
cache_alloc of 100000 K: 2.77343x    
