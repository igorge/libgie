//================================================================================================================================================
// FILE: exceptions.h
// (c) GIE 2010-07-07  18:41
//
//================================================================================================================================================
#ifndef H_GUARD_EXCEPTIONS_2010_07_07_18_41
#define H_GUARD_EXCEPTIONS_2010_07_07_18_41
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "debugger_break.hpp"

#include <boost/exception/all.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <vector>
//================================================================================================================================================
#define GIE_LOG_STDERR
//================================================================================================================================================
#define GIE_LOC_INFO BOOST_CURRENT_FUNCTION, __FILE__, __LINE__

#ifndef GIE_BUILD_RELEASE
    #define GIE_THROW(x)                                               \
	do {									\
		try{								\
			BOOST_THROW_EXCEPTION(x);				\
		}catch(boost::exception const& e) {				\
			::gie::exception::impl::generate_backtrace(e);           \
			::gie::exception::impl::log_exception_invokation(e);	\
			throw;							\
		}								\
	} while(false)								\
	/**/

	#define GIE_THROW_EX(x, function, file, line)	\
		do{ \
			try{\
				::boost::throw_exception( ::boost::enable_error_info(x) <<\
						::boost::throw_function(function) <<\
						::boost::throw_file(file) <<\
						::boost::throw_line((int)line) );\
			}catch(boost::exception const& e){	\
				::gie::exception::impl::generate_backtrace(e);           \
				::gie::exception::impl::log_exception_invokation(e);	\
				throw;\
			}\
		}while(false)
		/**/
#else
    #error "not impl"
    #define GIE_THROW(x) BOOST_THROW_EXCEPTION(x)
#endif


#define GIE_THROW_IF(cond, x) do {  if(cond) GIE_THROW(x); }while(false)

#define GIE_UNEXPECTED_IN_DTOR()    assert(false)

#define GIE_UNIMPLEMENTED()	do {\
	/*assert(!"UNIMPLEMENTED");*/	\
        ::gie::debug::brk(); \
	GIE_THROW(::gie::exception::unimplemented()); } while(false)

#define GIE_UNEXPECTED()	\
    /*assert(!"UNEXPECTED");*/	\
	GIE_THROW(::gie::exception::unexpected())

#define GIE_UNEXPECTED_EX(x1)	\
    /*assert(!"UNEXPECTED");*/	\
	GIE_THROW(::gie::exception::unexpected() << x1)

#define GIE_UNEXPECTED1(msg)\
	/*assert(!"UNEXPECTED");*/	\
	GIE_THROW(::gie::exception::unexpected() << ::gie::exception::error_str_einfo(msg))

#define GIE_UNEXPECTEDW1(msg)\
	/*assert(!"UNEXPECTED");*/	\
	GIE_THROW(::gie::exception::unexpected() << sd::exception::error_wstr_einfo(msg))

#define GIE_CHECK(x) do{ if(!(x)) { ::gie::debug::brk(); GIE_THROW(::gie::exception::condition_check_failed() << ::gie::exception::condition_check_expr_einfo( BOOST_PP_STRINGIZE(x) ));} } while(false)
#define GIE_CHECK_EX(x, e) if(!(x)) GIE_THROW(e << ::gie::exception::condition_check_expr_einfo( BOOST_PP_STRINGIZE(x) ) )

#define GIE_BT_BEGIN try{
#define GIE_BT_END	\
			} catch (boost::exception& e){	\
				::gie::append_bt(e, BOOST_CURRENT_FUNCTION, __LINE__ );	\
				throw;	\
			}	\
			/**/

#define GIE_GUARD_BEGIN try { GIE_BT_BEGIN
#define GIE_GUARD_END(handler) GIE_BT_END } catch(...) { handler(); }


namespace gie {

    namespace exception {

    	struct bt_t {
    		std::vector< std::tuple<std::string, unsigned int> > m;
    	};
    	inline std::ostream& operator<<(std::ostream&os, bt_t const&v){
    		os<<"\n";
    		for(size_t i=0; i<v.m.size(); ++i){
    			os<<"["<<i<<"] ("<< std::get<1>(v.m[i]) <<") "<<std::get<0>(v.m[i]) <<"\n";
    		}
    		return os;
    	}

        typedef boost::error_info< struct tag_str_einfo, std::string > error_str_einfo;
        typedef boost::error_info< struct tag_condition_check_expr_einfo, std::string > condition_check_expr_einfo;
        typedef boost::error_info< struct tag_bt_einfo, bt_t > bt_einfo;

        struct root : virtual boost::exception, virtual std::exception {};
        struct unexpected : virtual root { };
        struct unimplemented : virtual unexpected { };
        struct condition_check_failed : virtual unexpected { };


        namespace impl {

            template <class T> void generate_backtrace(T const& /*e*/) {}

            template <class T> void log_exception_invokation(T const& e ) {
               
            	fprintf(stderr, ( boost::diagnostic_information(e) +"\n").c_str() );

                //log4cplus::Logger const logger = log4cplus::Logger::getInstance("sd.debug");
                //LOG4CPLUS_DEBUG(logger, diag_info);
            }

        }


    } // end ns impl

    template <class E>
    void append_bt(E&e, std::string&& item, unsigned int line=0){
    	auto const bt_info = boost::get_error_info<exception::bt_einfo>(e);
    	if(bt_info){
    		bt_info->m.push_back( std::make_tuple(std::move(item), line) );
    	} else {
    		typename std::remove_reference<decltype(*bt_info)>::type bt_vec;
    		bt_vec.m.push_back( std::make_tuple(std::move(item), line) );
    		e << exception::bt_einfo( std::move( bt_vec ) );
    	}
    }

    inline
    std::string diagnostic_information_from_current_exception(){
    	try{
    		throw;
        } catch( boost::exception const& e ) {
            return diagnostic_information(e);
        } catch( std::exception  const& e ) {
           return std::string("std::exception: ")+e.what();
        } catch(...) {
            return ("unknown exception" );
        }
    }

    inline void e_filter_identity(){throw;}


    namespace impl {
    	template <class T, int dummy=0>
    	struct check_type{
        	BOOST_STATIC_ASSERT(   !std::is_reference<T>::value
        						|| (std::is_function<typename std::remove_reference<T>::type>::value));
        	BOOST_STATIC_ASSERT( !std::is_rvalue_reference<T>::value );
    	};

    	template <class T>
    	struct is_function_reference
    		: boost::mpl::bool_<std::is_reference<T>::value && std::is_function<typename std::remove_reference<T>::type>::value> {};

		template <class T>
		struct deduce_stored_param
			: boost::mpl::eval_if<
			  	  impl::is_function_reference<T>,
			  	  boost::mpl::identity<T>,
			  	  typename std::remove_reference<T>
			> {};

    }


    template <class FunHead,class FunTail>
    struct filter_chain_t
		: private impl::check_type<FunHead,0>
		, private impl::check_type<FunTail,1>
	{

    	typedef filter_chain_t<FunHead, FunTail> this_type;

    	template <class FunHeadC,class FunTailC>
    	filter_chain_t(FunHeadC&& head, FunTailC&& tail)
    		: m_head( std::forward<FunHeadC>(head) )
    		, m_tail( std::forward<FunTailC>(tail) )
    	{}



    	void operator()(){
    		try{
    			m_head();
    		}catch(...){
    			m_tail();
    		}
    	}

    	FunHead m_head;
    	FunTail m_tail;
    };

    template <class FunHead,class FunTail>
    filter_chain_t<
    	typename impl::deduce_stored_param<FunHead>::type,
    	typename impl::deduce_stored_param<FunTail>::type>
    filter_chain(FunHead&&head, FunTail&&tail){

    	typedef typename impl::deduce_stored_param<FunHead>::type head_t;
    	typedef typename impl::deduce_stored_param<FunTail>::type tail_t;

    	impl::check_type<head_t>();
    	impl::check_type<tail_t>();

    	return filter_chain_t<head_t, tail_t>( std::forward<FunHead>(head), std::forward<FunTail>(tail) );
    }

	template <
		class U,
		class V,
		class TailFun>
	filter_chain_t<filter_chain_t<U,V>, TailFun>
	operator % (filter_chain_t<U,V>&& HeadFun, TailFun&&tail){
		return filter_chain( std::move(HeadFun), std::forward<TailFun>(tail));
	}

    struct e_filter_chain{
    	template <class TailFun>
    	filter_chain_t<void(&)(),TailFun>
    	operator % (TailFun&&tail){
    		return filter_chain(e_filter_identity, std::forward<TailFun>(tail));
    	}

    };

    template <class EFilter, class Fun>
    void eval_and_filter(Fun&&fun, EFilter&&filter){
    	try{
    		fun();
    	} catch(...) {
    		filter();
    	}
    }

    template <class ExceptionToPass, class EFilter>
    void filter_all_except(EFilter&&filter){
    	try{
    		throw;
    	} catch (ExceptionToPass const&) {
    		throw;
    	} catch (...){
    		filter();
    	}
    }

    template <class ExceptionToPass, class ExceptionToPass2, class EFilter>
    void filter_all_except(EFilter&&filter){
    	try{
    		throw;
    	} catch (ExceptionToPass const&) {
    		throw;
    	} catch (ExceptionToPass2 const&) {
    		throw;
    	} catch (...){
    		filter();
    	}
    }

    template <class ExceptionToPass, class ExceptionToPass2, class EFilter>
    void filter_all_except_ignored(EFilter&&filter){
    	try{
    		throw;
    	} catch (ExceptionToPass const&) {
    	} catch (ExceptionToPass2 const&) {
    	} catch (...){
    		filter();
    	}
    }

}
//================================================================================================================================================
#endif
//================================================================================================================================================
