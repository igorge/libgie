//================================================================================================================================================
// FILE: simple_caching_allocator.h
// (c) GIE 2011-11-25  22:55
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_CACHING_ALLOCATOR_2011_11_25_22_55
#define H_GUARD_SIMPLE_CACHING_ALLOCATOR_2011_11_25_22_55
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "next_power_of_two.hpp"
#include "log/debug.hpp"

#include <vector>
//================================================================================================================================================
namespace gie {

    struct simple_default_allocator {
        void* allocate(std::size_t const size)const {
            return new char[size];
        }
        void deallocate(void* const pointer, size_t const size)const noexcept{
            delete[] static_cast<char*>(pointer);
        }
    };

    template <class SimpleAllocatorT>
    struct simple_allocator_by_ref_wrapper_t{

        simple_allocator_by_ref_wrapper_t(SimpleAllocatorT& allocator)
                :m_allocator(allocator)
        {
        }

        void* allocate(std::size_t const size)const {
            return m_allocator.allocate(size);
        }
        void deallocate(void* const pointer, size_t const size)const noexcept{
            return m_allocator.deallocate(pointer, size);
        }


        SimpleAllocatorT& m_allocator;
    };

    template <class SimpleAllocatorT>
    simple_allocator_by_ref_wrapper_t<SimpleAllocatorT>
    make_ref_wrapper(SimpleAllocatorT& allocator){ return simple_allocator_by_ref_wrapper_t<SimpleAllocatorT>(allocator); }

    struct simple_allocator_i {

        simple_allocator_i() = default;
        simple_allocator_i(simple_allocator_i const&) = delete;

        virtual void* allocate(std::size_t const size) = 0;
        virtual void deallocate(void* const pointer, size_t const size) noexcept = 0;
    };


    template <class SimpleAllocatorT>
    struct simple_allocator_to_i_adapter_t : simple_allocator_i {

        template <typename ... ARGS>
        explicit simple_allocator_to_i_adapter_t(ARGS ... args)
                :m_allocator( std::forward<ARGS>(args) ... )
        {
        }

        void* allocate(std::size_t const size) override {
            return m_allocator.allocate(size);
        }

        void deallocate(void* const pointer, size_t const size) noexcept override {
            return m_allocator.deallocate(pointer, size);
        }


        SimpleAllocatorT m_allocator;
    };


	struct simple_caching_allocator {

        friend struct boost_test__simple_caching_allocator;

		typedef void* pointer_type;

		explicit simple_caching_allocator(unsigned int const min_exponent = 4 /*16*/, unsigned int max_exponent = 13 /*8192*/)
            : m_min_exponent( min_exponent )
            , m_max_exponent( max_exponent)
		{
            assert (min_exponent>0);
            assert (max_exponent<32);
            assert (min_exponent<max_exponent);

            auto const buckets = m_max_exponent-min_exponent+1;

            m_buckets.resize(buckets);
		}

        simple_caching_allocator& operator=(simple_caching_allocator const&)=delete;
        simple_caching_allocator(simple_caching_allocator const&)=delete;
        simple_caching_allocator(simple_caching_allocator&&)=delete;

        ~simple_caching_allocator(){

            //GIE_DEBUG_TRACE();
            //GIE_DEBUG_LOG("Alive objects: " << m_alive_objects);

            assert(m_alive_objects==0); // to be sure, that all users of this allocator have returned all memory back

            for (auto const & bucket:m_buckets){
                for (auto&& pointer : bucket){
                    ::operator delete(pointer);
                }
            }

        }

//        void trace_buckets_(){
//            GIE_DEBUG_LOG("alive="<<m_alive_objects<< "[0]="<<m_buckets[0].size() << " [1]="<<m_buckets[1].size());
//        }

        void* allocate(std::size_t const size){

//              trace_buckets_();

//              GIE_DEBUG_IF_LOG(m_alive_objects>1024, "WARNING: To many alive objects: "<<m_alive_objects);

              int const idx = bucket_idx_from_size_(size);
              if(idx==-1){
                  //GIE_DEBUG_LOG("simple_caching_allocator: size "<<size<<" is to large for caching.");
                  return ::operator new(size);
              } else {
                  assert(static_cast<size_t>(idx) < m_buckets.size());

                  if (m_buckets[idx].empty()){
                      auto const effective_size = size_from_bucket_idx_(idx);
                      //GIE_DEBUG_LOG("Buckets empty for size: size "<<size<<"("<<effective_size<<").");
                      assert(effective_size>=size);
                      auto const tmp = ::operator new(effective_size);
                      ++m_alive_objects;
                      return tmp;
                  } else {
                      auto const tmp = m_buckets[idx].back();
                      m_buckets[idx].resize(m_buckets[idx].size()-1);
                      ++m_alive_objects;
                      return tmp;
                  }
              }

        }

        void deallocate(void* const pointer, size_t const size)noexcept {

            //GIE_DEBUG_IF_LOG(m_alive_objects>1024, "WARNING: To many alive objects: "<<m_alive_objects);

            try {
                int const idx = bucket_idx_from_size_(size);
                if(idx==-1){
                    //GIE_DEBUG_LOG("simple_caching_allocator: size "<<size<<" is to large for caching, deleting.");
                    do_deallocate(pointer, size);
                } else {
                    //GIE_DEBUG_LOG("Caching size: "<<size<<"("<<size_from_bucket_idx_(idx)<<").");
                    assert(static_cast<size_t>(idx) < m_buckets.size());
                    m_buckets[idx].push_back(pointer);
                    --m_alive_objects;
                }
            } catch (...){
                do_deallocate(pointer,size);
                GIE_UNEXPECTED_IN_DTOR();
            }
        }

        void do_deallocate(void* const pointer, size_t const size)const noexcept{
            ::operator delete(pointer);
        }

    private:

        size_t size_from_bucket_idx_(unsigned int idx)const{
            assert(idx <= m_max_exponent - m_min_exponent);

            auto const exp_size = idx + m_min_exponent;
            assert( exp_size < std::numeric_limits<size_t>::digits );

            return static_cast<size_t>(1) << exp_size;
        }

        int bucket_idx_from_size_(size_t const size)const {
            auto const power_of_two_exponent_size = effective_exponent_(size);
            if(power_of_two_exponent_size==-1){
                return -1;
            } else {
                assert (static_cast<unsigned int>(power_of_two_exponent_size) >=m_min_exponent);
                assert (static_cast<unsigned int>(power_of_two_exponent_size) <=m_max_exponent);

                auto const idx = power_of_two_exponent_size - m_min_exponent;
                return idx;
            }

        }

        int effective_exponent_(size_t const size)const {
            auto const power_of_two_exponent_size = next_power_of_two_as_exponent(size);

            if (power_of_two_exponent_size<m_min_exponent){
                return m_min_exponent;
            } else if (power_of_two_exponent_size>m_max_exponent){
                return -1;
            } else {
                return power_of_two_exponent_size;
            }

        }

    private:
        unsigned int const m_min_exponent;
        unsigned int const m_max_exponent;
	    std::vector< std::vector<pointer_type> > m_buckets;

        unsigned int m_alive_objects = 0;
	};



}
//================================================================================================================================================
#endif
//================================================================================================================================================
