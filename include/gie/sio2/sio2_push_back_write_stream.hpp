//================================================================================================================================================
// FILE: sio2_push_back_write_stream.h
// (c) GIE 2016-10-11  20:35
//
//================================================================================================================================================
#ifndef H_GUARD_SIO2_PUSH_BACK_WRITE_STREAM_2016_10_11_20_35
#define H_GUARD_SIO2_PUSH_BACK_WRITE_STREAM_2016_10_11_20_35
//================================================================================================================================================
#include "sio2_core.hpp"

#include <utility>
//================================================================================================================================================
namespace gie { namespace sio2 {


        template <class ContainerT>
        struct push_back_writer_t {

            typedef typename ContainerT::value_type value_type;

            template <class T>
            void write(T const& value){
                impl::require_octet<T>();
                impl::require_stream_target_octet_storage<value_type>();

                m_container.push_back( impl::force_truncate_octet(value) );
            }

            template <class T>
            auto& operator()(T&& value){
                serialize_out( std::forward<T>(value), *this );
                return *this;
            }

            ContainerT& m_container;
        };


} }
//================================================================================================================================================
#endif
//================================================================================================================================================
