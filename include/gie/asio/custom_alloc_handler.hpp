//================================================================================================================================================
// FILE: asio_custom_alloc_handler.h
// (c) GIE 2011-11-17  22:23
//
//================================================================================================================================================
#ifndef H_GUARD_ASIO_CUSTOM_ALLOC_HANDLER_2011_11_17_22_23
#define H_GUARD_ASIO_CUSTOM_ALLOC_HANDLER_2011_11_17_22_23
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "../log/debug.hpp"

#include <boost/static_assert.hpp>
//================================================================================================================================================
namespace gie {

	struct asio_allocator_t {

	  void* allocate(std::size_t size){
		  return ::operator new(size);
	  }

	  void deallocate(void* pointer, size_t const size){
		  ::operator delete(pointer);
	  }

	};

	  template <
	  	  typename Handler,
	  	  typename AsioAllocator
	  >
	  struct custom_alloc_handler
	  {
		typedef custom_alloc_handler<Handler, AsioAllocator> SelfT;

		BOOST_STATIC_ASSERT( !std::is_reference<Handler>::value );

		template <typename PHandle, typename PAsioAllocator>
		explicit
		custom_alloc_handler(PHandle&& h, PAsioAllocator&& asio_allocator)
		   : handler_( std::forward<PHandle>( h ) )
		   , allocator_( std::forward<PAsioAllocator>( asio_allocator ) )
		{
		}



		custom_alloc_handler(SelfT&& other)
		   : handler_( std::move( other.handler_ ) )
		   , allocator_ ( std::move(other.allocator_ ) )
		{
		}

		custom_alloc_handler(SelfT const & other)
		   : handler_( other.handler_ )
		   , allocator_ ( other.allocator_ )
		{
		}

		SelfT& operator=(SelfT const& other){

			this->handler_ = other.handler_;
			this->allocator_ = other.allocator_;

			return *this;
		}
		SelfT& operator=(SelfT && other){

			this->handler_ = std::move(other.handler_);
			this->allocator_ = std::move(other.allocator_);

			return *this;
		}

		template <typename... Args>
		void operator()(Args&&... args)const{
			handler_(std::forward<Args>(args)...);
		}

		friend
		void* asio_handler_allocate(std::size_t size, SelfT* this_handler){
		  GIE_DEBUG_LOG("ALLOCATING: "<<size);
		  return this_handler->allocator_.allocate(size);
		}

		friend
		void asio_handler_deallocate(void* pointer, std::size_t size, SelfT* this_handler){
		  GIE_DEBUG_LOG("DEALLOCATING: "<<size);
		  this_handler->allocator_.deallocate(pointer, size);
		}

	  private:
		Handler handler_;
		AsioAllocator allocator_;
	  };

	  template <typename Handler, typename AsioAllocator = asio_allocator_t>
	  custom_alloc_handler<
	  	  typename std::remove_reference<Handler>::type,
	  	  typename std::remove_reference<AsioAllocator>::type >
	  make_custom_alloc_handler(Handler&& h, AsioAllocator&& asio_allocator = asio_allocator_t() )  {
		return custom_alloc_handler<
					typename std::remove_reference<Handler>::type,
					typename std::remove_reference<AsioAllocator>::type >
						( std::forward<Handler>(h), std::forward<AsioAllocator>(asio_allocator) );
	  }


}
//================================================================================================================================================
#endif
//================================================================================================================================================
