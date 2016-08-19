//================================================================================================================================================
// FILE: tuple_apply.h
// (c) GIE 2011-09-14  01:25
//
//================================================================================================================================================
#ifndef H_GUARD_TUPLE_APPLY_2011_09_14_01_25
#define H_GUARD_TUPLE_APPLY_2011_09_14_01_25
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <boost/bind.hpp>

#include <tuple>
#include <functional>
//================================================================================================================================================
namespace gie {

	namespace impl {

		template<
			typename Ret,
			unsigned int idx_plus_one>
		struct apply_helper2;

		template<
			typename Ret>
		struct apply_helper2<Ret,0> {

			template<class Fun, class Tuple, typename... Args>
			static
			Ret _(Fun&&fun, Tuple&&tuple, Args&&...args){
				(void)tuple;
				return fun( std::forward<Args>(args)... );
			}

		};

		template<
			typename Ret,
			unsigned int idx_plus_one>
		struct apply_helper2 {

			template<class Fun, class Tuple, typename... Args>
			static
			Ret _(Fun&&fun, Tuple&&tuple, Args&&...args){

				return
					apply_helper2<Ret, idx_plus_one-1>::_(
							std::forward<Fun>(fun),
							std::forward<Tuple>(tuple),
							std::get<idx_plus_one-1>(tuple),
							std::forward<Args>(args)... );
			}

		};

	}

	template <
		typename Fun,
		typename... Args>
	typename std::result_of<Fun(Args...)>::type
	apply(Fun&&fun, std::tuple<Args...>&& tuple){

		return
		impl::apply_helper2<
			typename std::result_of<Fun(Args...)>::type,
			std::tuple_size< typename std::remove_reference<decltype(tuple)>::type >::value
		>
			::_(std::forward<Fun>(fun), std::move(tuple) );
	}

	template <
		typename Fun,
		typename... Args>
	typename std::result_of<Fun(Args...)>::type
	apply(Fun&&fun, std::tuple<Args...> const & tuple){

		return
		impl::apply_helper2<
			typename std::result_of<Fun(Args...)>::type,
			std::tuple_size<
				typename std::remove_cv<
					typename std::remove_reference<decltype(tuple)>::type>::type >::value
		>
			::_(std::forward<Fun>(fun), tuple );
	}

}
//================================================================================================================================================
#endif
//================================================================================================================================================
