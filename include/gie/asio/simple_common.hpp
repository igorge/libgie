//================================================================================================================================================
// FILE: simple_common.h
// (c) GIE 2011-08-30  05:38
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_COMMON_2011_08_30_05_38
#define H_GUARD_SIMPLE_COMMON_2011_08_30_05_38
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/log/debug.hpp"
#include "gie/exceptions.hpp"

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
//================================================================================================================================================
namespace gie {

	typedef boost::shared_ptr<boost::asio::io_service> shared_io_service_t;

	template <class Self>
	struct default_efilter_log_and_silence_exception {

		void efilter_log_and_silence_exception_(){
			try {
				throw;
			} catch( boost::exception & e ) {
				GIE_LOG( diagnostic_information(e) );
			} catch( std::exception & e ) {
				GIE_LOG( e.what() );
			} catch(...){ GIE_LOG("unknown exception type"); }
		}

	};


}
//================================================================================================================================================
#endif
//================================================================================================================================================
