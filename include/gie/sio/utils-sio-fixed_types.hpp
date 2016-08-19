//================================================================================================================================================
// FILE: utils-sio-fixed_types.h
// (c) GIE 2011-07-11  21:34
//
//================================================================================================================================================
#ifndef H_GUARD_UTILS_SIO_FIXED_TYPES_2011_07_11_21_34
#define H_GUARD_UTILS_SIO_FIXED_TYPES_2011_07_11_21_34
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "util-sio.hpp"

#include <boost/integer.hpp>
#include <boost/strong_typedef.hpp>
#include <limits>
//================================================================================================================================================
namespace gie { namespace sio {
    
    template <class T> struct base_type_for;
    template <class T> struct useful_bits_in;
    

    typedef unsigned char uint8_t;
    template <> struct base_type_for<uint8_t>{ typedef uint8_t type; };
    
    inline bool is_high_bit_set(uint8_t const v){
        return (v & 128);
    }
    
    namespace impl {
        typedef boost::int_t<4*8>::fast int32_fit_t;
        typedef boost::uint_t<4*8>::fast uint32_fit_t;
        typedef boost::uint_t<2*8>::fast uint16_fit_t;
    }
    
    BOOST_STRONG_TYPEDEF( impl::int32_fit_t, int32_le_t)
    template <> struct base_type_for<int32_le_t>{ typedef impl::int32_fit_t type; };
    template <> struct useful_bits_in<int32_le_t>{ static unsigned int const value = 32;};

    BOOST_STRONG_TYPEDEF( impl::uint32_fit_t, uint32_le_t)
    template <> struct base_type_for<uint32_le_t>{ typedef impl::uint32_fit_t type; };
    template <> struct useful_bits_in<uint32_le_t>{ static unsigned int const value = 32;};
    
    BOOST_STRONG_TYPEDEF( impl::uint16_fit_t, uint16_le_t)
    template <> struct base_type_for<uint16_le_t>{ typedef impl::uint16_fit_t type; };
    template <> struct useful_bits_in<uint16_le_t>{ static unsigned int const value = 32;};
    
    BOOST_STRONG_TYPEDEF( float, float32_t )
    template <> struct base_type_for<float32_t>{ typedef float type; };
    
    namespace impl {
        
        template<class T>
        struct integral_type_build_prepare;
        
        template<>
        struct integral_type_build_prepare<int32_le_t>{
            typedef base_type_for<int32_le_t>::type base_type;
            
            static int32_fit_t apply(bool const is_negative){
                BOOST_STATIC_ASSERT( std::numeric_limits<base_type>::is_signed );
                BOOST_STATIC_ASSERT( std::numeric_limits<base_type>::digits == useful_bits_in<int32_le_t>::value-1 );
                
                return 0;
            }            
        };
        
    }
    
        template <class Op>
        void serialize_in(int32_le_t& v, Op& op){
            uint8_t o1, o2, o3, o4;
            op(o1)(o2)(o3)(o4);
            
            v = impl::integral_type_build_prepare< int32_le_t >::apply( is_high_bit_set(o4) )
                        | o1
                        | (o2<<8)
                        | (o3<<16)
                        | (o4<<24);
                
        }
    
        
        template <class Op>
        void serialize_in(uint32_le_t& v, Op& op){
            uint8_t o1, o2, o3, o4;
            op(o1)(o2)(o3)(o4);
            
            v = 
                  static_cast< base_type_for<uint32_le_t>::type > ( o1 )
                | (o2<<8)
                | (o3<<16)
                | (o4<<24);
                
        }

        
        template <class Op>
        void serialize_in(uint16_le_t& v, Op& op){
            uint8_t o1, o2;
            op(o1)(o2);
            
            v = 
                  static_cast< base_type_for<uint16_le_t>::type > ( o1 )
                | (o2<<8);
                
        }
        
        template <class Op>
        void serialize_in(float32_t& v, Op& op){
            base_type_for<float32_t>::type & vv = v ;
            BOOST_STATIC_ASSERT( sizeof(vv)==4 );
            op.read(reinterpret_cast<uint8_t*>( &vv ), sizeof(vv) );
           
                
        }
        
    
} }
//================================================================================================================================================
#endif
//================================================================================================================================================
