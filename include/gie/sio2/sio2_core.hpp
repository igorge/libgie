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
#include "gie/config.hpp"
#include "gie/type_util.hpp"

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
            struct int16_le : integral_type {};
            struct uint16_le : integral_type {};
            struct int32_le : integral_type {};
            struct uint32_le : integral_type {};

            struct float32_le : integral_type {};

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
            return { value };
        }

        template <class TTag, class T>
        typename std::enable_if<std::is_convertible<TTag, tag::type_tag>::value , as_t<TTag, T const> >::type
        as(T&& value){
            return { value };
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
            constexpr bool require_integral(){
                static_assert(std::is_integral<T>::value, "should be integral type");
                static_assert(std::numeric_limits<T>::radix==2, "the only one radix");
                return true;
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



            template <unsigned int bits_count, class T>
            typename std::enable_if< gie::bits_count<T>() <= bits_count, T>::type // nothing to pad
            pad_high_bits_with_one(T const& v){
                return v;
            };

            template <unsigned int bits_count, class T>
            typename std::enable_if< (gie::bits_count<T>() > bits_count), T>::type
            pad_high_bits_with_one(T const& v){

                static_assert ( require_integral<T>() );
                static_assert ( !std::numeric_limits<T>::is_signed );

                auto const bits_to_pad = gie::bits_count<T>() - bits_count;
                static_assert (bits_to_pad > 0);
                static_assert (bits_count < gie::bits_count<T>());

                return { ((std::numeric_limits<T>::max() >> bits_count) << bits_count) | v };
            };

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


        // out -- uint16_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::uint16_le, T>&& v, WriteStream& write_stream){
            static_assert(impl::require_integral<T>());
            static_assert(!std::numeric_limits<T>::is_signed);
            static_assert(std::numeric_limits<T>::digits>=16);

            constexpr auto r_shift = (std::numeric_limits<T>::digits-8);
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-8)) >> r_shift  );
            write_stream.write( (v.value << (std::numeric_limits<T>::digits-16)) >> r_shift );
        }

        // in -- uint16_le
        template <class ReadStream, class T>
        void serialize_in(as_t<tag::uint16_le, T>&& v, ReadStream& read_stream){
            static_assert(impl::require_integral<T>());
            static_assert(!std::numeric_limits<T>::is_signed);
            static_assert(std::numeric_limits<T>::digits>=16);

            T const o1 = read_stream.read();
            T const o2 = read_stream.read();

            v.value =  ( o1 | (o2<<8) );
        };


        // out -- int16_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::int16_le, T>&& v, WriteStream& write_stream){
            static_assert (impl::require_integral<T>());
            static_assert (std::numeric_limits<T>::is_signed);
            static_assert (gie::bits_count<T>()>=16);

            using u_t = typename uint_from_int<T>::type;
            static_assert( impl::require_integral<u_t>() );

            serialize_out(as<tag::uint16_le>(static_cast<u_t>(v.value) ), write_stream);

        }

        // in -- int16_le
        template <class ReadStream, class T>
        void serialize_in(as_t<tag::int16_le, T>&& v, ReadStream& read_stream){
            static_assert (impl::require_integral<T>());
            static_assert (std::numeric_limits<T>::is_signed);
            static_assert (gie::bits_count<T>()>=16);

            using u_t = typename uint_from_int<T>::type;
            static_assert( impl::require_integral<u_t>() );

            u_t const o1 = read_stream.read();
            u_t const o2 = read_stream.read();

            if(o2 & 0b10000000) { //signed
                v.value = impl::pad_high_bits_with_one<16> ( o1 | (o2<<8) ) ;
            } else {
                v.value = ( o1 | (o2<<8) );
            }

        };



        // out -- uint32_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::uint32_le, T>&& v, WriteStream& write_stream){
            static_assert(impl::require_integral<T>());
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
            static_assert(impl::require_integral<T>());
            static_assert(!std::numeric_limits<T>::is_signed);
            static_assert(std::numeric_limits<T>::digits>=32);

            T const o1 = read_stream.read();
            T const o2 = read_stream.read();
            T const o3 = read_stream.read();
            T const o4 = read_stream.read();

            v.value =  ( o1 | (o2<<8) | (o3<<16) | (o4<<24) );
        };


        // out -- int32_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::int32_le, T>&& v, WriteStream& write_stream){
            static_assert (impl::require_integral<T>());
            static_assert (std::numeric_limits<T>::is_signed);
            static_assert (gie::bits_count<T>()>=32);

            using u_t = typename uint_from_int<T>::type;
            static_assert( impl::require_integral<u_t>() );

            serialize_out(as<tag::uint32_le>(static_cast<u_t>(v.value) ), write_stream);

        }

        // in -- int32_le
        template <class ReadStream, class T>
        void serialize_in(as_t<tag::int32_le, T>&& v, ReadStream& read_stream){
            static_assert (impl::require_integral<T>());
            static_assert (std::numeric_limits<T>::is_signed);
            static_assert (gie::bits_count<T>()>=32);

            using u_t = typename uint_from_int<T>::type;
            static_assert( impl::require_integral<u_t>() );

            u_t const o1 = read_stream.read();
            u_t const o2 = read_stream.read();
            u_t const o3 = read_stream.read();
            u_t const o4 = read_stream.read();

            if(o4 & 0b10000000) { //signed
               v.value = impl::pad_high_bits_with_one<32> ( o1 | (o2<<8) | (o3<<16) | (o4<<24) ) ;
            } else {
               v.value = ( o1 | (o2<<8) | (o3<<16) | (o4<<24) );
            }

        };



        namespace impl {

            template <unsigned int B1, unsigned int B2, unsigned int B3, unsigned int B4>
            struct byte_order32{
                using b1 = std::integral_constant<unsigned int, B1>;
                using b2 = std::integral_constant<unsigned int, B2>;
                using b3 = std::integral_constant<unsigned int, B3>;
                using b4 = std::integral_constant<unsigned int, B4>;
            };

            #if defined(GIE_FLOAT_BYTE_ORDER__LITTLE)
                using float32_le_byte_mapper = byte_order32<0,1,2,3>;
            #elif defined(GIE_FLOAT_BYTE_ORDER__BIG)
                using float32_le_byte_mapper = byte_order32<3,2,1,0>;
            #else
                #error Unknown byte order
            #endif

        }


        // out -- float32_le
        template <class WriteStream, class T>
        void serialize_out(as_t<tag::float32_le, T>&& v, WriteStream& write_stream){
            static_assert (std::numeric_limits<T>::is_iec559);
            static_assert (sizeof(T)==4 );

            auto const bytes = reinterpret_cast<unsigned char const*>(&v.value);

            write_stream.write( bytes[ impl::float32_le_byte_mapper::b1::value ] );
            write_stream.write( bytes[ impl::float32_le_byte_mapper::b2::value ] );
            write_stream.write( bytes[ impl::float32_le_byte_mapper::b3::value ] );
            write_stream.write( bytes[ impl::float32_le_byte_mapper::b4::value ] );
        }


        // in -- float32_le
        template <class WriteStream, class T>
        void serialize_in(as_t<tag::float32_le, T>&& v, WriteStream& stream) {
            static_assert(std::numeric_limits<T>::is_iec559);
            static_assert(sizeof(T) == 4);

            auto const bytes = reinterpret_cast<unsigned char *>(&v.value);

            bytes[ impl::float32_le_byte_mapper::b1::value ] = stream.read();
            bytes[ impl::float32_le_byte_mapper::b2::value ] = stream.read();
            bytes[ impl::float32_le_byte_mapper::b3::value ] = stream.read();
            bytes[ impl::float32_le_byte_mapper::b4::value ] = stream.read();

        }



}}
//================================================================================================================================================
#endif
//================================================================================================================================================
