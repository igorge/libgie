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
#include <type_traits>
#include <algorithm>
//================================================================================================================================================
namespace gie {

    template <class T, class U>
    constexpr bool require_same_radix(){
        static_assert( std::is_integral<T>::value );
        static_assert( std::is_integral<U>::value );
        static_assert( std::numeric_limits<T>::radix == std::numeric_limits<U>::radix );
        return true;
    }


    //return number of bits in a T (including sign)
    template <class T>
    constexpr unsigned int bits_count(){
        static_assert( std::is_integral<T>::value );
        return (std::numeric_limits<T>::is_signed?1:0) + std::numeric_limits<T>::digits;
    }

    // returns unsigned type capable of holding T values
    template <class T>
    struct uint_from_int{
        typedef typename boost::uint_t<bits_count<T>()>::fast type;
    };

    // returns unsigned integral type that is capable of holding T and U positive bits (not counting sign value if any)
    template <class T, class U>
    struct uint_can_hold_digits {

        static_assert(require_same_radix<T,U>());

        constexpr static auto d1 = std::numeric_limits<T>::digits;
        constexpr static auto d2 = std::numeric_limits<U>::digits;

        typedef typename boost::uint_t<std::max(d1,d2)>::fast type;

        static_assert(require_same_radix<T,type>());
    };


    // returns unsigned integral type that is capable of holding T and U value
    template <class T, class U>
    struct uint_max_t {

        static_assert(require_same_radix<T,U>());

        constexpr static auto d1 = bits_count<T>();
        constexpr static auto d2 = bits_count<U>();

        typedef typename boost::uint_t<std::max(d1,d2)>::fast type;

        static_assert(require_same_radix<T,type>());
    };


}
//================================================================================================================================================
#endif
//================================================================================================================================================
