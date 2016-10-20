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
#include "gie/allocator/simple_to_std_allocator.hpp"
#include "gie/allocator/simple_to_std_aligned_allocator.hpp"
//================================================================================================================================================
namespace gie {


    template <class StdAllocatorT>
    struct std_to_simple_allocator_t {
        typedef typename std::allocator_traits<StdAllocatorT>::template rebind_alloc<char> byte_allocator_t;

        explicit std_to_simple_allocator_t(StdAllocatorT const& a)
                : m_std_allocator(a)
        {}

        void* allocate(std::size_t const size){
            return static_cast<void*>( m_std_allocator.allocate(size) );
        }

        void deallocate(void* const pointer, size_t const size)noexcept {
            m_std_allocator.deallocate(static_cast<char*>(pointer), size);
        }


    private:
        byte_allocator_t m_std_allocator;
    };

    template <class StdAllocatorT>
    std_to_simple_allocator_t<StdAllocatorT>
    std_allocator_to_simple(StdAllocatorT const& a){
        return std_to_simple_allocator_t<StdAllocatorT>{a};
    }



//    template <class T, class SimpleAllocatorT>
//    using simple_to_std_allocator_t = simple_to_std_non_aligned_allocator_t<T, SimpleAllocatorT>;

    template <class T, class SimpleAllocatorT>
    using simple_to_std_allocator_t = simple_to_std_aligned_allocator_t<T, SimpleAllocatorT>;


}
//================================================================================================================================================
#endif
//================================================================================================================================================
