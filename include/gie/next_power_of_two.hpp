//================================================================================================================================================
// FILE: next_power_of_two.h
// (c) GIE 2011-11-23  18:48
//
//================================================================================================================================================
#ifndef H_GUARD_NEXT_POWER_OF_TWO_2011_11_23_18_48
#define H_GUARD_NEXT_POWER_OF_TWO_2011_11_23_18_48
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "debugger_break.hpp"

#include <boost/static_assert.hpp>
#include <boost/preprocessor/cat.hpp>

#include <assert.h>
#include <limits.h>
#include <limits>
//================================================================================================================================================
namespace gie {


#define GIE_DEF_BIT_FUNCS(T, suffix) 	\
	inline 								\
	unsigned int get_first_low_bit(T const v){ /*return pos+1 or 0 if not found*/ 	\
		return BOOST_PP_CAT(__builtin_ffs, suffix)(v); 								\
	} 									\
	/**/ 								\
	inline 								\
	unsigned int get_first_high_bit(T const v){ /*return pos+1 or 0 if not found*/ 	\
		if(!v)							\
			return 0;					\
		else							\
			return sizeof(v)*CHAR_BIT - BOOST_PP_CAT(__builtin_clz, suffix)(v);		\
	}									\
	/**/

	GIE_DEF_BIT_FUNCS(unsigned int,)
	GIE_DEF_BIT_FUNCS(unsigned long,l)
	GIE_DEF_BIT_FUNCS(unsigned long long,ll)


	template <class T>
	T generic_next_power_of_two(T k) {

		BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

		if (k == 0)
			return 1;

		k--;

		for( T i=1; i<sizeof(T)*CHAR_BIT; i<<=1 ){
			k = k | k >> i;
		}

		assert( k<std::numeric_limits<T>::max() );

		return  k+1;
	}

	template <class T>
	unsigned int gcc_next_power_of_two_as_exponent(T const v) {
		BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

		auto const h_b = get_first_high_bit(v);
		if(!h_b){
			return 0;
		} else {
			return get_first_low_bit(v)==h_b?h_b-1:h_b;
		}
	}


	template <class T>
	T gcc_next_power_of_two(T const v) {
		BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

		auto const exp = gcc_next_power_of_two_as_exponent(v);
		assert( exp < static_cast<decltype(exp)>( std::numeric_limits<T>::digits ) );

		return static_cast<T>(1) << exp;
	}

	template <class T>
	T next_power_of_two(T const v) {
		auto const tmp = gcc_next_power_of_two(v);
		assert(tmp==generic_next_power_of_two(v));

		return tmp;

	}


	template <class T>
	T next_power_of_two_as_exponent(T const v, unsigned int const bit_pos) {

		auto const tmp = gcc_next_power_of_two(v);
		assert(tmp==generic_next_power_of_two(v));

		return tmp;

	}


}
//================================================================================================================================================
#endif
//================================================================================================================================================
