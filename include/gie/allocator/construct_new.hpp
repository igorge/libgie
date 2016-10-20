//================================================================================================================================================
// FILE: construct_new.h
// (c) GIE 2016-10-20  02:42
//
//================================================================================================================================================
#ifndef H_GUARD_CONSTRUCT_NEW_2016_10_20_02_42
#define H_GUARD_CONSTRUCT_NEW_2016_10_20_02_42
//================================================================================================================================================
#pragma once
//================================================================================================================================================
namespace gie {

    template <class T, class AllocatorT, class... Args>
    typename std::enable_if<std::is_same<T,typename AllocatorT::value_type>::value, T*>::type construct_new(AllocatorT& alloc, Args&& ...args){

        T * const ptr = alloc.allocate(1);

        try {
            std::allocator_traits<AllocatorT>::construct(alloc, ptr, std::forward<Args>(args)...);
        } catch (...){
            alloc.deallocate(ptr, 1);
            throw;
        }

        return ptr;
    };


    template <class T, class AllocatorT, class... Args>
    typename std::enable_if< ! std::is_same<T,typename AllocatorT::value_type>::value, T*>::type construct_new(AllocatorT& alloc, Args&& ...args) {
        typedef typename std::allocator_traits<AllocatorT>::template rebind_alloc<T> rebound_allocator_t;
        rebound_allocator_t rebound_allocator{alloc};
        return construct_new<T>(rebound_allocator, std::forward<Args>(args)... );
    };


    template <class T, class AllocatorT>
    typename std::enable_if<std::is_same<T,typename AllocatorT::value_type>::value, void>::type destroy_free(AllocatorT& alloc, T* const p){
        if(p) {
            std::allocator_traits<AllocatorT>::destroy(alloc, p);
            alloc.deallocate(p, 1);
        }
    }

    template <class T, class AllocatorT>
    typename std::enable_if< ! std::is_same<T,typename AllocatorT::value_type>::value, void>::type destroy_free(AllocatorT& alloc, T* const p){
        typedef typename std::allocator_traits<AllocatorT>::template rebind_alloc<T> rebound_allocator_t;
        rebound_allocator_t rebound_allocator{alloc};
        destroy_free(rebound_allocator, p);
    }


}
//================================================================================================================================================
#endif
//================================================================================================================================================
