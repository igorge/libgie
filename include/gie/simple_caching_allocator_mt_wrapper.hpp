//================================================================================================================================================
// FILE: simple_caching_allocator_mt_wrapper.h
// (c) GIE 2016-10-15  23:51
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_CACHING_ALLOCATOR_MT_WRAPPER_2016_10_15_23_51
#define H_GUARD_SIMPLE_CACHING_ALLOCATOR_MT_WRAPPER_2016_10_15_23_51
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <boost/thread/mutex.hpp>
//================================================================================================================================================
namespace gie {

    template <class T>
    struct simple_mt_allocator_t{

        friend class boost_test__simple_caching_allocator;

        template <class... Args>
        simple_mt_allocator_t(Args&&... args) : m_allocator_( std::forward<Args>(args)... ) {}

        void* allocate(std::size_t const size){
            boost::mutex::scoped_lock lock{m_mutex};
            return m_allocator_.allocate(size);
        }

        void deallocate(void* const pointer, size_t const size)noexcept {
            boost::mutex::scoped_lock lock{m_mutex};
            return m_allocator_.deallocate(pointer, size);
        }

    private:
        T m_allocator_;
        boost::mutex m_mutex;
    };

}
//================================================================================================================================================
#endif
//================================================================================================================================================
