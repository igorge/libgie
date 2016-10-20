//================================================================================================================================================
// FILE: simple_to_std_allocator.h
// (c) GIE 2016-10-20  02:09
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_TO_STD_ALLOCATOR_2016_10_20_02_09
#define H_GUARD_SIMPLE_TO_STD_ALLOCATOR_2016_10_20_02_09
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/allocator/construct_new.hpp"
#include "gie/log/debug.hpp"

#include <type_traits>
//================================================================================================================================================
namespace gie {

    template <class T, class SimpleAllocatorT>
    struct simple_to_std_non_aligned_allocator_t {
        typedef T * pointer;
        typedef T const * const_pointer;
        typedef T & reference;
        typedef T const & const_reference;
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        template <typename T1, typename T2> friend class simple_to_std_non_aligned_allocator_t;

        template <typename U>
        struct rebind {
            typedef simple_to_std_non_aligned_allocator_t<U, SimpleAllocatorT> other;
        };

        explicit simple_to_std_non_aligned_allocator_t(SimpleAllocatorT& simple_allocator)
                : m_simple_allocator(simple_allocator)
        {
            //GIE_DEBUG_TRACE_INOUT();
        };

        template <class U> simple_to_std_non_aligned_allocator_t(const simple_to_std_non_aligned_allocator_t<U, SimpleAllocatorT>& other) : simple_to_std_non_aligned_allocator_t(other.m_simple_allocator) {
            //GIE_DEBUG_TRACE_INOUT();
        }


        bool check_alignment(void* const pointer, size_t const size, size_t const count)const{
            auto p = pointer;
            auto sz = size;
            p = std::align(alignof(T), sizeof(T)*count, p, sz);

            return pointer == p;
        }

        T* allocate(std::size_t n)const{
            //GIE_DEBUG_LOG("Allocating "<<n*sizeof( T )<< " bytes");

            auto const buffer_in_bytes =  n * sizeof( T );

            void * const buffer = m_simple_allocator.allocate(buffer_in_bytes); //rely on ::operator new() to allocate memory suitable for any type
            assert(check_alignment(buffer, buffer_in_bytes, n));

            T * const p = static_cast< T* >( buffer );
            return p;
        }

        void deallocate(T* p, std::size_t n)const{
            //GIE_DEBUG_LOG("Deallocating "<<n*sizeof( T )<< " bytes");

            m_simple_allocator.deallocate(p,n * sizeof( T ));
        }

        template <class TT, class... Args>
        TT* alloc_(Args&& ...args)const{
            return gie::construct_new<TT>(*this, std::forward<Args>(args)...);
        };

        template <class TT>
        void dealloc_(TT* const p)const {
            gie::destroy_free(*this, p);
        }


    private:

        SimpleAllocatorT& m_simple_allocator;

    };


    template <class T, class U, class SimpleAllocatorT>
    bool operator==(const simple_to_std_non_aligned_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_non_aligned_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) == static_cast<void*>(&r.m_simple_allocator);
    };

    template <class T, class U, class SimpleAllocatorT>
    bool operator!=(const simple_to_std_non_aligned_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_non_aligned_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) != static_cast<void*>(&r.m_simple_allocator);
    };

}
//================================================================================================================================================
#endif
//================================================================================================================================================
