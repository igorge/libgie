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
#include <utility>
#include <array>
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

        template <class TTag, class T>
        struct as_array_of_t {
            typedef TTag target_type_tag;
            typedef T source_type;

            T& value;
        };

        template <class TTag, class T>
        as_array_of_t<TTag, T>
        as_array_of(T& value){
            return as_array_of_t<TTag, T>{ value };
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
            constexpr void require_stream_source_octet_storage(){
                require_integral<T>();
                static_assert(bits_count<T>() >=8, "must contain at least 8 bits");
            }



            //
            // force_truncate_octet
            //
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


        // std::array & T[N] array serialize
        //
        template <class Stream, class T, std::size_t N>
        void serialize(std::array<T, N> & v, Stream& stream){
            for(auto && i : v){
                stream(i);
            }
        }

        template <class Stream, class T, std::size_t N>
        void serialize(T (&v)[N], Stream& stream){
            for(auto && i : v){
                stream(i);
            }
        }

        template <class Stream, class Tag, class T, std::size_t N>
        void serialize(as_array_of_t<Tag, std::array<T, N> > && v, Stream& stream){
            for(auto && i : v.value){
                stream( as<Tag>(i) );
            }
        }

        template <class Stream, class Tag, class T, std::size_t N>
        void serialize(as_array_of_t<Tag, T[N]> && v, Stream& stream){
            for(auto && i : v.value){
                stream( as<Tag>(i) );
            }
        }


        // dispatch serialize_[in|out] via generic serialize
        //
        template <class WriteStream, class T>
        void serialize_out(T&& v, WriteStream& write_stream){
            return serialize (std::forward<T>(v), write_stream);
        }

        template <class ReaderStream, class T>
        void serialize_in(T&& v, ReaderStream& read_stream){
            return serialize (std::forward<T>(v), read_stream);
        }



        // out -- octet
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::octet, T>&& v, WriteStream& write_stream){
            impl::require_octet<T>();
            write_stream.write(v.value);
        }

        // in -- octet
        template <class ReadStream, class T>
        void serialize_in(as_t<tag::octet, T>&& v, ReadStream& read_stream){
            impl::require_octet<T>();

            v.value = read_stream.read();
        }

        // out -- uint32_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::uint32_le, T>&& v, WriteStream& write_stream){
            impl::require_integral<T>();
            static_assert(!std::numeric_limits<T>::is_signed);
            static_assert(std::numeric_limits<T>::digits>=32);

            constexpr auto r_shift = (std::numeric_limits<T>::digits-8);
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-8)) >> r_shift  );
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-16)) >> r_shift );
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-24)) >> r_shift );
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-32)) >> r_shift );
        }

        // in -- uint32_le
        template <class ReadStream, class T>
        void serialize_in(as_t<tag::uint32_le, T>&& v, ReadStream& read_stream){
            impl::require_integral<T>();
            static_assert(!std::numeric_limits<T>::is_signed);
            static_assert(std::numeric_limits<T>::digits>=32);

            T const o1 = read_stream.read();
            T const o2 = read_stream.read();
            T const o3 = read_stream.read();
            T const o4 = read_stream.read();

            v.value =  ( o1 | (o2<<8) | (o3<<16) | (o4<<24) );
        };


    }}
//================================================================================================================================================
#endif
//================================================================================================================================================
