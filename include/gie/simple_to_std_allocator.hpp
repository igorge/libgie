//================================================================================================================================================
// FILE: simple_to_std_allocator.h
// (c) GIE 2016-10-16  00:48
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_TO_STD_ALLOCATOR_2016_10_16_00_48
#define H_GUARD_SIMPLE_TO_STD_ALLOCATOR_2016_10_16_00_48
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "log/debug.hpp"

#include <type_traits>
//================================================================================================================================================
namespace gie {

    template <class T, class SimpleAllocatorT>
    struct simple_to_std_allocator_t {
        typedef T * pointer;
        typedef T const * const_pointer;
        typedef T & reference;
        typedef T const & const_reference;
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        template <typename T1, typename T2> friend class simple_to_std_allocator_t;

        template <typename U> using  rebind = simple_to_std_allocator_t<U, SimpleAllocatorT>;

        explicit simple_to_std_allocator_t(SimpleAllocatorT& simple_allocator)
                : m_simple_allocator(simple_allocator)
        {
            //GIE_DEBUG_TRACE_INOUT();
        };

        template <class U> simple_to_std_allocator_t(const simple_to_std_allocator_t<U, SimpleAllocatorT>& other) : simple_to_std_allocator_t(other.m_simple_allocator) {
            //GIE_DEBUG_TRACE_INOUT();
        }

        T* allocate(std::size_t n){
            //GIE_DEBUG_LOG("Allocating "<<n*sizeof( T )<< " bytes");

            T * const p = static_cast< T* >( m_simple_allocator.allocate(n * sizeof( T )) );
            return p;
        }

        void deallocate(T* p, std::size_t n){
            //GIE_DEBUG_LOG("Deallocating "<<n*sizeof( T )<< " bytes");

            m_simple_allocator.deallocate(p,n * sizeof( T ));
        }


        template <class TT, class... Args>
        typename std::enable_if<!std::is_same<T,TT>::value, TT*>::type alloc_(Args&& ...args){
            rebind<TT> alloc{*this};
            TT* const ptr = alloc.allocate(1);

            auto const tmp = new (ptr)TT{std::forward<Args>(args)...};
            assert(ptr==tmp);

            return tmp;
        }

        template <class TT, class... Args>
        typename std::enable_if<std::is_same<T,TT>::value, TT*>::type alloc_(Args&& ...args){
            TT* const ptr = this->allocate(1);

            auto const tmp = new (ptr)TT{std::forward<Args>(args)...};
            assert(ptr==tmp);

            return tmp;
        }

        template <class TT>
        typename std::enable_if<!std::is_same<T,TT>::value, void >::type dealloc_(TT* const p){
            rebind<TT> alloc{*this};
            alloc.deallocate(p, 1);
        }

        template <class TT>
        typename std::enable_if<std::is_same<T,TT>::value, void >::type dealloc_(TT* const p){
            this->deallocate(p, 1);
        }


    private:

        SimpleAllocatorT& m_simple_allocator;

    };


    template <class T, class U, class SimpleAllocatorT>
    bool operator==(const simple_to_std_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) == static_cast<void*>(&r.m_simple_allocator);
    };

    template <class T, class U, class SimpleAllocatorT>
    bool operator!=(const simple_to_std_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) != static_cast<void*>(&r.m_simple_allocator);
    };


}
//================================================================================================================================================
#endif
//================================================================================================================================================
