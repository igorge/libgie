//================================================================================================================================================
// FILE: simple_to_std_aligned_allocator.h
// (c) GIE 2016-10-20  02:09
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_TO_STD_ALIGNED_ALLOCATOR_2016_10_20_02_09
#define H_GUARD_SIMPLE_TO_STD_ALIGNED_ALLOCATOR_2016_10_20_02_09
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/log/debug.hpp"

#include <type_traits>
//================================================================================================================================================
namespace gie {

    auto const alloc_marker_no_alignment_overhead = 0xCCu;
    auto const alloc_marker_alignment_overhead_in_next_byte = 0xBBu;

    template <class T, class SimpleAllocatorT>
    struct simple_to_std_aligned_allocator_t {
        typedef T * pointer;
        typedef T const * const_pointer;
        typedef T & reference;
        typedef T const & const_reference;
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        template <typename T1, typename T2> friend class simple_to_std_aligned_allocator_t;

        template <typename U>
        struct rebind {
            typedef simple_to_std_aligned_allocator_t<U, SimpleAllocatorT> other;
        };

        explicit simple_to_std_aligned_allocator_t(SimpleAllocatorT& simple_allocator)
                : m_simple_allocator(simple_allocator)
        {
            //GIE_DEBUG_TRACE_INOUT();
        };

        template <class U> simple_to_std_aligned_allocator_t(const simple_to_std_aligned_allocator_t<U, SimpleAllocatorT>& other) : simple_to_std_aligned_allocator_t(other.m_simple_allocator) {
            //GIE_DEBUG_TRACE_INOUT();
        }


        void write_extra_(void * const buffer, size_t const data_size, unsigned char b1, unsigned char b2)const{
            assert(buffer);

            unsigned char * const p = static_cast<unsigned char*>(buffer);
            p[data_size] = b1;
            p[data_size+1] = b2;
        }

        std::pair<unsigned int, unsigned int> read_extra_(void * const buffer, size_t const data_size)const{
            assert(buffer);

            unsigned char * const p = static_cast<unsigned char*>(buffer);

            return {p[data_size], p[data_size+1]};
        };

        const size_t extra_size = 2; // [marker][alignment padding]

        T* allocate(std::size_t n)const{
            //GIE_DEBUG_LOG("Allocating "<<n*sizeof( T )<< " bytes");

            static_assert( alignof(T) <= std::numeric_limits<unsigned char>::max() );

            auto const data_size_in_bytes =  n * sizeof( T );
            auto const buffer_in_bytes_plus_alignment_overhead = data_size_in_bytes + alignof(T);
            auto const buffer_in_bytes_plus_extra = buffer_in_bytes_plus_alignment_overhead + extra_size;

            void * const buffer = m_simple_allocator.allocate(buffer_in_bytes_plus_extra);

            auto p = buffer;
            auto sz = buffer_in_bytes_plus_alignment_overhead;

            p = std::align(alignof(T), data_size_in_bytes, p, sz);

            assert( p );

            if(p==buffer){
                write_extra_(buffer, data_size_in_bytes, alloc_marker_no_alignment_overhead, 0);
                return static_cast< T* >( buffer );
            } else {
                auto const diff = buffer_in_bytes_plus_alignment_overhead - sz;
                assert(diff<=std::numeric_limits<unsigned char>::max());

                write_extra_(p, data_size_in_bytes, alloc_marker_alignment_overhead_in_next_byte, diff);
                return static_cast< T* >( p );
            }
        }


        void deallocate(T* p, std::size_t n)const{
            //GIE_DEBUG_LOG("Deallocating "<<n*sizeof( T )<< " bytes");

            auto const data_size_in_bytes =  n * sizeof( T );
            auto const buffer_in_bytes_plus_alignment_overhead = data_size_in_bytes + alignof(T);
            auto const buffer_in_bytes_plus_extra = buffer_in_bytes_plus_alignment_overhead + extra_size;

            auto const& extra = read_extra_(p,data_size_in_bytes);
            assert( (extra.first==alloc_marker_no_alignment_overhead) || (extra.first==alloc_marker_alignment_overhead_in_next_byte) ); //memory corrupted on assert

            if(extra.first==alloc_marker_no_alignment_overhead){

                m_simple_allocator.deallocate(p, buffer_in_bytes_plus_extra);

            } else if(extra.first==alloc_marker_alignment_overhead_in_next_byte) {

                assert(extra.second<=std::numeric_limits<unsigned char>::max());
                auto const corrected_pointer = static_cast<unsigned char*>(static_cast<void*>(p)) - extra.second;

                m_simple_allocator.deallocate(corrected_pointer, buffer_in_bytes_plus_extra);
            }

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
    bool operator==(const simple_to_std_aligned_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_aligned_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) == static_cast<void*>(&r.m_simple_allocator);
    };

    template <class T, class U, class SimpleAllocatorT>
    bool operator!=(const simple_to_std_aligned_allocator_t<T, SimpleAllocatorT>& l, const simple_to_std_aligned_allocator_t<U, SimpleAllocatorT>& r){
        GIE_DEBUG_TRACE();
        return static_cast<void*>(&l.m_simple_allocator) != static_cast<void*>(&r.m_simple_allocator);
    };

}
//================================================================================================================================================
#endif
//================================================================================================================================================
