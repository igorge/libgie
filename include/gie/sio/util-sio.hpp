//================================================================================================================================================
// FILE: util-sio.h
// (c) GIE 2010-09-08  16:26
//
//================================================================================================================================================
#ifndef H_GUARD_UTIL_SIO_2010_09_08_16_26
#define H_GUARD_UTIL_SIO_2010_09_08_16_26
//================================================================================================================================================
#pragma once
//================================================================================================================================================
//#include "util-raw-buffer-oiter.hpp"
#include "util-sio-exceptions.hpp"

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/ref.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/type_traits/is_same.hpp>

#include <limits>
//================================================================================================================================================
#define GIE_SIO_GEN( type_tag, fields_seq )  \
    /*type definition*/ \
    struct type_tag { \
        BOOST_PP_SEQ_FOR_EACH_I( GIE_SIO_GEN_ITEM_DEF, 0/*aux data*/, fields_seq ) \
    }; \
    /*serialization code*/ \
    template <class Op> \
    void serialize(type_tag& v, Op& op){ \
        op \
        BOOST_PP_SEQ_FOR_EACH_I( GIE_SIO_GEN_SER_DEF, 0/*aux data*/, fields_seq ) \
        ;\
    } \
    /*ostream printer*/ \
    inline std::ostream& operator<<(std::ostream& os, type_tag const& v){ \
        os << "{"; \
        BOOST_PP_SEQ_FOR_EACH_I( GIE_SIO_GEN_PRINTER, 0/*aux data*/, fields_seq ) \
        os << "}"; \
        return os; \
    } \
/**/
    
#define GIE_SIO_GEN_ITEM_DEF(r, aux/*aux data*/, i/*iter counter*/, type_def /*data*/)   \
            ::boost::mpl::identity< BOOST_PP_TUPLE_ELEM(2/*size*/, 0/*extract idx*/, type_def /*tuple*/) >::type    \
            BOOST_PP_TUPLE_ELEM(2/*size*/, 1/*extract idx*/, type_def /*tuple*/);    \
            /**/
    
#define GIE_SIO_GEN_SER_DEF(r, aux/*aux data*/, i/*iter counter*/, type_def /*data*/)   \
        (v. BOOST_PP_TUPLE_ELEM(2/*size*/, 1/*extract idx*/, type_def /*tuple*/) ) 
/**/

#define GIE_SIO_GEN_PRINTER(r, aux/*aux data*/, i/*iter counter*/, type_def /*data*/)   \
        BOOST_PP_IF(i, os <<", " , ; ) ;\
        os << \
 BOOST_PP_STRINGIZE( BOOST_PP_TUPLE_ELEM(2/*size*/, 1/*extract idx*/, type_def /*tuple*/) ) << "="\
 << ::gie::sio::impl::printer_wrap(v. BOOST_PP_TUPLE_ELEM(2/*size*/, 1/*extract idx*/, type_def /*tuple*/) ) \
        ;\
/**/

//================================================================================================================================================

namespace gie {
    
    namespace sio {
        
        
        namespace impl {
            
            template <class T> struct printer_wrap_t {
                printer_wrap_t(T const& p_d) : d(p_d) {}
                
                T const& d;
            };

            template <class T> 
            printer_wrap_t<T> printer_wrap(T const& t){ return printer_wrap_t<T> (t); }
            
            template <class T>
            std::ostream& operator << (std::ostream& os, printer_wrap_t<T> const &  v){
                os << v.d;
                return os;
            }
            
            template <class T, size_t N>
            std::ostream& operator << (std::ostream& os, printer_wrap_t<T [N]> const &  v){
                os << '[';
                
                if( boost::begin(v.d)!=boost::end(v.d) ){
                    os << printer_wrap( v.d[0] );
                }
                std::for_each( boost::begin(v.d)+1, boost::end(v.d), [&os](T const&e){
                    os << ", "<< printer_wrap( e );
                });
                
                os << ']';
                
                return os;
            }

            inline std::ostream& operator << (std::ostream& os, printer_wrap_t<unsigned char> const &  v){
                return os << static_cast<unsigned int>( v.d );
            }
            
            
        }
        
        
        typedef unsigned char uint8_t;
        
        template <class IStream, class SelfT>
        struct octet_istream_impl_t {
            
            typedef IStream istream_t;
            
            octet_istream_impl_t() {
                BOOST_STATIC_ASSERT( std::numeric_limits<uint8_t>::digits == 8 );
                BOOST_STATIC_ASSERT( !std::numeric_limits<uint8_t>::is_signed );
                BOOST_STATIC_ASSERT(sizeof(uint8_t)==sizeof(typename istream_t::char_type));
            }
            
            uint8_t read(){
                GIE_CHECK_EX( this->stream_().good(), exception::underflow() );
                
                typename IStream::char_type tmp;
                this->stream_().read( &tmp, 1 );
                
                GIE_CHECK_EX( !this->stream_().fail(), exception::underflow() );
                GIE_CHECK_EX( !this->stream_().eof(), exception::underflow() );
                
                return tmp;
            }

            
            void read(uint8_t *v, size_t const N){
                assert(N);
                
                GIE_CHECK_EX( this->stream_().good(), exception::underflow() );
                
                this->stream_().read( reinterpret_cast<typename istream_t::char_type*>( v ), N );
                
                GIE_CHECK_EX( !this->stream_().fail(), exception::underflow() );
                GIE_CHECK_EX( !this->stream_().eof(), exception::underflow() );
            }
            
            template <size_t N>
            void read(uint8_t (&v)[N]){
                BOOST_STATIC_ASSERT( N );
                
                GIE_CHECK_EX( this->stream_().good(), exception::underflow() );
                
                this->stream_().read( reinterpret_cast<typename istream_t::char_type*>( &v[0] ), N );
                
                GIE_CHECK_EX( !this->stream_().fail(), exception::underflow() );
                GIE_CHECK_EX( !this->stream_().eof(), exception::underflow() );
            }
            
        private:
            istream_t& stream_(){ 
                BOOST_STATIC_ASSERT( (boost::is_same<istream_t&, decltype(this->self_()->stream_()) >::value) );
                
                return this->self_()->stream(); 
            }
            
            SelfT* self_(){ return static_cast<SelfT*>(this); }
        };
        
        template <class IStream>
        struct octet_istream_t : octet_istream_impl_t<IStream, octet_istream_t<IStream> > {
            typedef IStream istream_t;
            
            template <class T1> octet_istream_t(T1&& p1) : m_stream( std::forward<T1>(p1) ) {}
            template <class T1, class T2> octet_istream_t(T1&& p1, T2&& p2) : m_stream( std::forward<T1>(p1), std::forward<T2>(p2) ) {}
            
            istream_t& stream(){ return m_stream; }

        private:
            istream_t m_stream;
                
        };

        
        
        template <class IStream>
        struct octet_istream_wrap_t : octet_istream_impl_t<IStream, octet_istream_wrap_t<IStream> > {
            typedef IStream istream_t;
            
            octet_istream_wrap_t(IStream& p1) : m_stream( p1 ) {}
            
            istream_t& stream(){ return m_stream; }

        private:
            istream_t& m_stream;
                
        };
        
        // dispatch thought general definition or fail
        template <class Op, class T>
        void serialize_in(T& v, Op& op){
            return serialize(v, op);
        }

        
        template <class IOctStream>
        struct reader_t {
            
            typedef reader_t<IOctStream> this_type;
            
            reader_t(IOctStream& is) : m_is(is) {}
        
            template <class T>    
            this_type& operator()(T& v){
                
                serialize_in(v, *this);
                
                return *this;                        
            }
            
            void read(uint8_t&v){ v = this->m_is.read(); }

            void read(uint8_t* v, size_t const N){ this->m_is.read(v, N); }
            
            template <size_t N> 
            void read(uint8_t (&v)[N]){ this->m_is.read(v); }
            
        private:
            IOctStream&  m_is;
        };
        
        
        //serialize primitive types
        template <class Op>
        void serialize_in(uint8_t& v, Op& op){
            op.read( v );
        }
        
        template <class Op, size_t N>
        void serialize_in(uint8_t (&v)[N], Op& op){
            op.read(v);
        }

        
        
        //
        // generic array in serializer
        //
        template <class Op, class T, size_t N>
        void serialize_in(T (&v)[N], Op& op){
            boost::for_each(v, [&op](T& elem){
                
                op(elem);
                
            });
        }
        
        
    } //end ns sio
    
} // end ns gie
//================================================================================================================================================
#endif
//================================================================================================================================================
