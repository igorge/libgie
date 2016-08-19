//================================================================================================================================================
// FILE: util-scope-exit.h
// (c) GIE 2011-02-20  02:05
//
//================================================================================================================================================
#ifndef H_GUARD_UTIL_SCOPE_EXIT_2011_02_20_02_05
#define H_GUARD_UTIL_SCOPE_EXIT_2011_02_20_02_05
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "exceptions.hpp"
#include "debug.hpp"

#include <boost/preprocessor/cat.hpp>
//================================================================================================================================================
namespace gie {

	template <class FunT>
	struct scope_guard_t
	{

		explicit scope_guard_t(FunT && fun) : m_fun( std::move(fun) ){}

		scope_guard_t(scope_guard_t<FunT> && other) : m_fun( std::move(other.m_fun) ){}
		scope_guard_t(scope_guard_t<FunT> const& other) : m_fun( other.m_fun ){}

		~scope_guard_t(){

			try{
				(void)(*this);
				
				m_fun();

			}catch(...){
				GIE_UNEXPECTED_IN_DTOR();
			}

		}
	private:
		scope_guard_t<FunT>& operator=(scope_guard_t<FunT> const&);
	private:
		FunT m_fun;
	};

	template <class FunT>	
	scope_guard_t<FunT> make_guard(FunT && fun){
		return scope_guard_t<FunT>( std::move(fun) );
	}


#define GIE_SCOPE_EXIT __attribute__ ((unused))  auto const& BOOST_PP_CAT(sg__stcrypt__, __COUNTER__) = ::gie::make_guard
	
}
//================================================================================================================================================
#endif
//================================================================================================================================================
