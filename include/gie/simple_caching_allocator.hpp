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

		typedef void* pointer_type;

		simple_caching_allocator(unsigned int const first_bucket_size)
			: m_first_bucket_size( first_bucket_size )
		{
			assert( m_first_bucket_size==next_power_of_two(m_first_bucket_size) );

		}

		  void* allocate(std::size_t size){
			  return ::operator new(size);
		  }

		  void deallocate(void* pointer, size_t const size){
			  ::operator delete(pointer);
		  }

	private:
		  unsigned int bucket_idx_from_size(size_t const size){
			  auto const power_of_two_size = next_power_of_two(size);
			  //auto const
			  GIE_DEBUG_LOG("size: "<< size << ", " << power_of_two_size);
		  }
	private:
		  unsigned int m_first_bucket_size;
		  std::vector< std::vector<pointer_type> > m_buckets;
	};



}
//================================================================================================================================================
#endif
//================================================================================================================================================
