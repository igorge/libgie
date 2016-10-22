//================================================================================================================================================
// FILE: type_util.h
// (c) GIE 2016-10-22  19:02
//
//================================================================================================================================================
#ifndef H_GUARD_TYPE_UTIL_2016_10_22_19_02
#define H_GUARD_TYPE_UTIL_2016_10_22_19_02
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <boost/integer.hpp>
#include <limits>
//================================================================================================================================================
namespace gie {

    template <class T, class U>
    constexpr bool require_same_radix(){
        static_assert( std::is_integral<T>::value );
        static_assert( std::is_integral<U>::value );
        static_assert( std::numeric_limits<T>::radix == std::numeric_limits<U>::radix );
        return true;
    };


    template <class T, class U>
    struct uint_can_hold_digits {

        static_assert(require_same_radix<T,U>());

        constexpr static auto d1 = std::numeric_limits<T>::digits;
        constexpr static auto d2 = std::numeric_limits<U>::digits;

        typedef typename boost::uint_t<std::max(d1,d2)>::fast type;

        static_assert(require_same_radix<T,type>());

    };

}
//================================================================================================================================================
#endif
//================================================================================================================================================
