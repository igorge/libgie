//================================================================================================================================================
// FILE: log.h
// (c) GIE 2011-07-11  15:21
//
//================================================================================================================================================
#ifndef H_GUARD_LOG_2011_07_11_15_21
#define H_GUARD_LOG_2011_07_11_15_21
//================================================================================================================================================
#pragma once
//================================================================================================================================================

#if defined(GIE_CONF_USE_BOOSTLOG)
    #include <boost/log/trivial.hpp>

    #define GIE_LOG_BACKEND(x) BOOST_LOG_TRIVIAL(info) << x
    #define GIE_DEBUG_LOG_BACKEND(x) BOOST_LOG_TRIVIAL(debug) << x
#elif defined(GIE_CONF_USE_G3LOG)
    #include <g3log/g3log.hpp>

    #define GIE_LOG_BACKEND(x) LOG(INFO) << x
    #define GIE_DEBUG_LOG_BACKEND(x) LOG(DEBUG) << x

#else
    #define GIE_LOG_BACKEND(x) GIE_LOG_SIMPLE(x)
    #define GIE_DEBUG_LOG_BACKEND(x) GIE_LOG("DEBUG: " << x)
#endif
//================================================================================================================================================


#include <boost/current_function.hpp>

#include <sstream>
#include <cstdio>

#include <assert.h>

//#define GIE_LOGGER_DO_NOT_LOG_THREAD_ID

#ifndef GIE_LOGGER_DO_NOT_LOG_THREAD_ID
	#include <boost/thread.hpp>

	#define GIE_IMPL_LOGGER_LOG_TID "["<< ::boost::this_thread::get_id() << "] " <<
#else
	#define GIE_IMPL_LOGGER_LOG_TID
#endif

#ifdef GIE_LOGGER_USE_LEVELS
	#ifndef GIE_LOGGER_NEED_TLS
		#define GIE_LOGGER_NEED_TLS
	#endif
#endif

#ifdef GIE_LOGGER_NEED_TLS
#include <boost/thread/tss.hpp>
#endif


#ifndef GIE_LOGGER_DO_NOT_LOG_TIME
	#include <boost/date_time.hpp>

	#define GIE_IMPL_LOGGER_LOG_TIME "["<< to_iso_string( boost::posix_time::microsec_clock::universal_time() ) << "] " <<
#else
	#define GIE_IMPL_LOGGER_LOG_TIME
#endif

//================================================================================================================================================
#ifdef GIE_LOGGER_NEED_TLS
namespace gie { namespace logger { namespace impl {

	struct per_thread_info_t {
		size_t const m_magic;

		per_thread_info_t()
			: m_magic( sizeof(per_thread_info_t) )
			#ifdef GIE_LOGGER_USE_LEVELS
			, m_level(0)
			, m_per_level_tab(4)
			#endif
		{}

		~per_thread_info_t(){
			assert(m_magic==sizeof(per_thread_info_t));
		}

		#ifdef GIE_LOGGER_USE_LEVELS
		unsigned int m_level;
		unsigned int m_per_level_tab;
		#endif

	};

} } }
#endif

namespace gie { namespace logger { namespace impl {
#ifdef GIE_LOGGER_USE_LEVELS

	struct ostream_wrapper_t {

		ostream_wrapper_t(std::ostream& os)
			: m_os( os )
		{}

		template<class T>
		ostream_wrapper_t& operator<<(T&&val){
			m_os << val;
			return *this;
		}


	private:
		std::ostream& m_os;
	};

	template<class OStream, class T>
	OStream& output_with_level(OStream& os, T&& val){
		os << val;
		return *os;
	}

#else

	template<class OStream>
	OStream& ostream_wrapper_t(OStream& os){
		return os;
	}

#endif
} } }
//================================================================================================================================================
#ifndef GIE_LOG_OPEN_TAG
	#define GIE_LOG_OPEN_TAG "[<!**["
#endif
#ifndef GIE_LOG_CLOSE_TAG
	#define GIE_LOG_CLOSE_TAG "]**!>]"
#endif

#define GIE_LOG_STDERR_EXT(msg, file, line)  \
        do{ try {               \
                std::ostringstream ostr;        \
                ::gie::logger::impl::ostream_wrapper_t(ostr) << "LOG: " << GIE_IMPL_LOGGER_LOG_TIME GIE_IMPL_LOGGER_LOG_TID  "["<<file<<":"<<line<<"] " << GIE_LOG_OPEN_TAG <<  msg << GIE_LOG_CLOSE_TAG << "\n"; \
                std::fputs( ostr.str().c_str(), stderr );  \
				std::fflush(stderr); \
                } catch(...) { ::gie::debug::exception_while_formatting_logging_message(); } \
        } while(false) \
        /**/

#define GIE_LOG_SIMPLE(msg) GIE_LOG_STDERR_EXT(msg, __FILE__, __LINE__)
#define GIE_LOG(x) GIE_LOG_BACKEND(x)

namespace gie { namespace debug {

    inline void exception_while_formatting_logging_message(){
        assert(false);
    }
    
} }


namespace gie { namespace logger {

        std::shared_ptr<void> init_logging_to_stderr();

#if defined(GIE_CONF_USE_BOOSTLOG)
#elif defined(GIE_CONF_USE_G3LOG)
#else
        inline std::shared_ptr<void> init_logging_to_stderr(){ return std::shared_ptr<void>(nullptr); }
#endif


} }
//================================================================================================================================================
#endif
//================================================================================================================================================
