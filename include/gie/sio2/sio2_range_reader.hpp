//================================================================================================================================================
// FILE: sio2_range_reader.h
// (c) GIE 2016-10-12  23:34
//
//================================================================================================================================================
#ifndef H_GUARD_SIO2_RANGE_READER_2016_10_12_23_34
#define H_GUARD_SIO2_RANGE_READER_2016_10_12_23_34
//================================================================================================================================================
#include "sio2_core.hpp"
#include "sio2_exceptions.hpp"

#include <utility>

#include <boost/range/iterator.hpp>
#include <boost/integer.hpp>
//================================================================================================================================================
namespace gie { namespace sio2 {


        template <class ForwardRangeT>
        struct range_reader_t {

            typedef typename boost::range_value<ForwardRangeT const>::type value_type;
            typedef typename boost::uint_t< impl::bits_count<value_type>() >::fast unsigned_value_type; // can hold all bits of value_type
            typedef typename boost::range_iterator<ForwardRangeT const>::type iterator_type;
            typedef std::uint_fast8_t octet_type;

            octet_type read(){
                impl::require_octet<octet_type>();
                impl::require_octet<unsigned_value_type>();
                impl::require_stream_source_octet_storage<value_type>();

                if(m_current==m_end) throw exception::underflow();

                unsigned_value_type const unsigned_value = *m_current;
                ++m_current;

                return impl::force_truncate_octet( static_cast<octet_type>(unsigned_value) );
            }

            template <class T>
            auto& operator()(T&& value){
                serialize_in( std::forward<T>(value), *this );
                return *this;
            }

            void test(iterator_type i){}

            template <class RangeT>
            range_reader_t(RangeT const & range)
                : m_current(range.cbegin())
                , m_end(range.cend())
            {

            }

        private:
            iterator_type m_current;
            iterator_type const m_end;

        };


    } }
//================================================================================================================================================
#endif
//================================================================================================================================================
