/**
    List

    Copyright 2021 Phil Bouchard <phil@fornux.com>

    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt
*/


#ifndef LIST_HPP
#define LIST_HPP


#include "intrusive_list.hpp"


namespace fornux
{


template <typename T, typename A = std::allocator<T>>
    struct list
    {
        struct iterator;
        
        list(size_t s = 0) : s(s)
        {
            for (size_t i = 0; i < s; ++ i)
                emplace_back();
        }
        
        size_t size()
        {
            return s;
        }
        
        template <typename... Args>
            void emplace_back(Args &&... args)
            {
                ++ s;
            
                node_t * p = a.allocate(1);
                
                new (p) node_t{std::forward<Args>(args)...};
            
                elements.push_back(& p->list_node);
            }
        
        void pop_back()
        {
            -- s;
            
            erase(rbegin());
        }
        
        void erase(iterator const & p)
        {
            -- s;
            
            p.p->~node_t();

            a.deallocate(p.p, 1);
        }
        
        iterator begin()
        {
            return & boost::smart_ptr::detail::classof(& node_t::list_node, elements.begin())->element;
        }

        iterator end()
        {
            return & boost::smart_ptr::detail::classof(& node_t::list_node, elements.end())->element;
        }
        
        iterator rbegin()
        {
            return & boost::smart_ptr::detail::classof(& node_t::list_node, elements.rbegin())->element;
        }

        iterator rend()
        {
            return & boost::smart_ptr::detail::classof(& node_t::list_node, elements.rend())->element;
        }
        
        ~list()
        {
            for (boost::smart_ptr::detail::intrusive_list::iterator<node_t, & node_t::list_node> m = elements.begin(), n = elements.begin(); m != elements.end(); m = n)
            {
                ++ n;
                delete &* m;
            }
        }

    private:
        size_t s{};
        boost::smart_ptr::detail::intrusive_list elements{};        

        struct node_t
        {
            boost::smart_ptr::detail::intrusive_list_node list_node;
            T element;
            
            template <typename... Args>
                node_t(Args &&... args)
                : element{std::forward<Args>(args)...}
                {
                }
        };

        typename A::template rebind<node_t>::other a;
        
    public:
        struct iterator
        {
            node_t * p;
            
            iterator()
            : p(nullptr)
            {
            }
            
            iterator(T * q)
            : p(boost::smart_ptr::detail::classof(& node_t::element, q))
            {
            }
            
            iterator(iterator const & p)
            : p(p.p)
            {
            }
            
            iterator & operator ++ ()
            {
                p = boost::smart_ptr::detail::classof(& node_t::list_node, p->list_node.next);
                
                return * this;
            }
            
            iterator & operator -- ()
            {
                p = boost::smart_ptr::detail::classof(& node_t::list_node, p->list_node.prev);
                
                return * this;
            }
            
            iterator & operator = (T * q)
            {
                p = boost::smart_ptr::detail::classof(& node_t::element, q);
                
                return * this;
            }
            
            iterator & operator = (iterator const & q)
            {
                p = q.p;
                
                return * this;
            }
            
            T & operator * ()
            {
                return p->element;
            }

            T * operator -> ()
            {
                return & p->element;
            }
            
            bool operator == (iterator const & q) const
            {
                return p == q.p;
            }
            
            bool operator != (iterator const & q) const
            {
                return p != q.p;
            }
        };
    };


}


#endif
