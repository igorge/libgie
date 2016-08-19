//================================================================================================================================================
// FILE: debug.h
// (c) GIE 2011-07-11  15:25
//
//================================================================================================================================================
#ifndef H_GUARD_DEBUG_2011_07_11_15_25
#define H_GUARD_DEBUG_2011_07_11_15_25
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "log.hpp"

#include <boost/current_function.hpp>
#include <boost/preprocessor/cat.hpp>
//================================================================================================================================================
#ifndef GIE_CONF_DISABLE_DEBUG_LOG
	#define GIE_DEBUG_LOG_EXT(x, file, line)  GIE_LOG_EXT("DEBUG: " << x, file, line)
	#define GIE_DEBUG_LOG(x)  GIE_LOG("DEBUG: " << x)
	#define GIE_DEBUG_IF_LOG(cond,x) do { if(cond){ GIE_DEBUG_LOG(x); }  }while(false)
	#define GIE_DEBUG_TRACE() GIE_DEBUG_LOG( BOOST_CURRENT_FUNCTION )
	#define GIE_DEBUG_TRACE1(x) GIE_DEBUG_LOG( BOOST_CURRENT_FUNCTION << " : "<< x)
	#define GIE_DEBUG_SCOPE_EXIT_LOG(x) GIE_SCOPE_EXIT([]{ GIE_DEBUG_LOG(x); })
	#define GIE_DEBUG_TRACE_INOUT() ::gie::debug::tracer BOOST_PP_CAT(sg__stcrypt__, __COUNTER__) (__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#else
	#define GIE_DEBUG_LOG(x) 			((void)0)
	#define GIE_DEBUG_IF_LOG(cond,x) 	((void)0)
	#define GIE_DEBUG_TRACE() 			((void)0)
	#define GIE_DEBUG_TRACE1(x) 		((void)0)
	#define GIE_DEBUG_SCOPE_EXIT_LOG(x) ((void)0)
	#define GIE_DEBUG_TRACE_INOUT() 	((void)0)
#endif
//================================================================================================================================================
namespace gie { namespace debug {

	struct tracer {
		tracer(char const * const file, unsigned int const line, char const * const func)
			: m_file( file )
			, m_line( line )
			, m_func( func ) {
			GIE_DEBUG_LOG_EXT("--> "<<m_func, m_file, m_line);
			//STCRYPT_LOG_INC_LEVEL();
		}
		~tracer(){

			//STCRYPT_LOG_DEC_LEVEL();
			GIE_DEBUG_LOG_EXT("<-- "<<m_func, m_file, m_line);
		}
	private:
		char const * const m_file;
		unsigned int const m_line;
		char const * const m_func;
	};


//================================================================================================================================================

} }
#endif
//================================================================================================================================================
