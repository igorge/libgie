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

	struct simple_caching_allocator {


        friend class boost_test__simple_caching_allocator;

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

        ~simple_caching_allocator(){
            for (auto const & bucket:m_buckets){
                for (auto&& pointer : bucket){
                    ::operator delete(pointer);
                }
            }
        }

		  void* allocate(std::size_t size){

              int const idx = bucket_idx_from_size_(size);
              if(idx==-1){
                  GIE_DEBUG_LOG("simple_caching_allocator: size "<<size<<" is to large for caching.");
                  return ::operator new(size);
              } else {
                  assert(idx < m_buckets.size());

                  if (m_buckets[idx].empty()){
                      auto const effective_size = size_from_bucket_idx_(idx);
                      assert(effective_size>=size);
                      return ::operator new(effective_size);
                  } else {
                      auto const tmp = m_buckets[idx].back();
                      m_buckets[idx].resize(m_buckets[idx].size()-1);
                      return tmp;
                  }
              }

		  }

        void deallocate(void* const pointer, size_t const size)noexcept {
            try {
                int const idx = bucket_idx_from_size_(size);
                if(idx==-1){
                    GIE_DEBUG_LOG("simple_caching_allocator: size "<<size<<" is to large for caching, deleting.");
                    ::operator delete(pointer);
                } else {
                    assert(idx < m_buckets.size());
                    m_buckets[idx].push_back(pointer);
                }
            } catch (...){
                ::operator delete(pointer);
                GIE_UNEXPECTED_IN_DTOR();
            }
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
	};



}
//================================================================================================================================================
#endif
//================================================================================================================================================
