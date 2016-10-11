//================================================================================================================================================
// FILE: sio2_integral.h
// (c) GIE 2016-10-11  01:18
//
//================================================================================================================================================
#ifndef H_GUARD_SIO2_INTEGRAL_2016_10_11_01_18
#define H_GUARD_SIO2_INTEGRAL_2016_10_11_01_18
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <type_traits>
#include <cinttypes>
#include <limits>
//================================================================================================================================================
namespace gie { namespace sio2 {


        // serialize_[in|out] --    define when generic is not applicable,
        //                          generic version dispatches via serialize(...)

        namespace tag {
            struct type_tag {};

            struct not_specified : type_tag {};

            struct integral_type : type_tag {};
            struct octet : integral_type {};
            struct int32_le : integral_type {};
            struct uint32_le : integral_type {};

        }

        template <class TTag, class T>
        struct as_t {
            typedef TTag target_type_tag;
            typedef T source_type;

            T& value;
        };

        template <class TTag, class T>
        typename std::enable_if<std::is_convertible<TTag, tag::type_tag>::value , as_t<TTag, T> >::type
        as(T& value){
            return as_t<TTag, T>{ value };
        }

        std::uint_fast8_t const octet_mask=0xFF;

        namespace impl {

            template <class T>
            constexpr void require_integral(){
                static_assert(std::is_integral<T>::value, "should be integral type");
                static_assert(std::numeric_limits<T>::radix==2, "the only one radix");
            }

            template <class T>
            constexpr int bits_count(){
                require_integral<T>();
                return (std::numeric_limits<T>::is_signed?1:0) + std::numeric_limits<T>::digits;
            }

            template <class T>
            constexpr void require_octet(){
                require_integral<T>();
                static_assert(!std::numeric_limits<T>::is_signed, "must not be signed");
                static_assert(std::numeric_limits<T>::digits>=8, "must contain at least 8 bits");
            }

            template <class T>
            constexpr void require_stream_target_octet_storage(){
                require_integral<T>();
                //static_assert(!std::numeric_limits<T>::is_signed, "must not be signed"); // can be signed, thou not strictly c++ standard conforming
                static_assert(bits_count<T>() >=8, "must contain at least 8 bits");
            }


            template <class T>
            typename std::enable_if<bits_count<T>()==8, T>::type
             force_truncate_octet(T value){
                require_octet<T>();
                return value;
            }

            template <class T>
            typename std::enable_if<(bits_count<T>() > 8), T>::type
            force_truncate_octet(T value){
                require_octet<T>();
                return value & static_cast<T>(octet_mask);
            }

        }




        template <class WriteStream, class T>
        void serialize_out(T& v, WriteStream& write_stream){
            return serialize(v, write_stream);
        }

        template <class ReaderStream, class T>
        void serialize_in(T& v, ReaderStream& read_stream){
            return serialize(v, read_stream);
        }





        template <class WriteStream, class T>
        void serialize_out(as_t<tag::octet, T>&& v, WriteStream& write_stream){
            impl::require_octet<T>();
            write_stream.write(v.value);
        }

        template <class WriteStream, class T>
        void serialize_out(as_t<tag::uint32_le, T>&& v, WriteStream& write_stream){
            impl::require_integral<T>();
            static_assert(!std::numeric_limits<T>::is_signed);


            //write_stream.write(v.value);
        }



//        template <class TTag, class ReadStream, class T>
//        typename std::enable_if<std::is_same<TTag, tag::octet_type>::value , void>::type
//        serialize_in(T& v, ReadStream& op){
//            typedef typename ReadStream::octet_type octet_type;
//            octet_type const value = op.read();
//
//            static_assert(!std::numeric_limits<octet_type>::is_signed, "octet must not be signed");
//            static_assert(std::numeric_limits<octet_type>::digits>=8, "octet must contain atleast 8 bits");
//
//            assert(std::numeric_limits<T>::digits==8 || (value & ~static_cast<octet_type>(octet_mask))==0 );
//
//            static_assert(!std::numeric_limits<T>::is_signed, "target octet type must not be signed");
//            static_assert(std::numeric_limits<T>::digits>=8, "target octet type must contain atleast 8 bits");
//
//            v = static_cast<T>( value );
//        }



    }}
//================================================================================================================================================
#endif
//================================================================================================================================================
