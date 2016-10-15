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
#include "gie/exceptions.hpp"
#include "debugger_break.hpp"

#include <boost/static_assert.hpp>
#include <boost/preprocessor/cat.hpp>

#include <assert.h>
#include <limits.h>
#include <limits>
//================================================================================================================================================
// round to next power of two
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
	std::pair<unsigned int,unsigned int> generic_get_first_low_high(T v){ //return pos+1 or 0 if not found
        BOOST_STATIC_ASSERT( std::numeric_limits<T>::radix==2 );
        BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

        if(v==0) return std::make_pair(0,0);

        unsigned int lease_set_idx = 0;
        unsigned int shift_count = 0;

        while(v && (lease_set_idx==0)){
            if(v & 1 == 1) lease_set_idx = shift_count+1;

            v>>=1;
            ++shift_count;
        }

        while(v){
            v>>=1;
            ++shift_count;
        }


        return std::make_pair(lease_set_idx,shift_count);


    }


	template <class T>
	unsigned int generic_next_power_of_two_as_exponent(T const v) {

		BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

        auto const lh_b = generic_get_first_low_high(v);
        if(!lh_b.first){
            return 0;
        } else {
            return lh_b.first==lh_b.second?lh_b.second-1:lh_b.second;
        }

	}

    template <class T>
    T generic_next_power_of_two(T const v) {
        BOOST_STATIC_ASSERT( !std::numeric_limits<T>::is_signed );

        auto const exp = generic_next_power_of_two_as_exponent(v);
        GIE_CHECK_EX( exp < static_cast<decltype(exp)>( std::numeric_limits<T>::digits ), exception::overflow_error() );

        return static_cast<T>(1) << exp;
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
        GIE_CHECK_EX( exp < static_cast<decltype(exp)>( std::numeric_limits<T>::digits ), exception::overflow_error() );

		return static_cast<T>(1) << exp;
	}




	template <class T>
	T next_power_of_two(T const v) {
		return gcc_next_power_of_two(v);;

	}


	template <class T>
	unsigned int next_power_of_two_as_exponent(T const v) {
		return gcc_next_power_of_two_as_exponent(v);
	}


}
//================================================================================================================================================
#endif
//================================================================================================================================================
