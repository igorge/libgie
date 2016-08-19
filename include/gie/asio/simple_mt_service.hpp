//================================================================================================================================================
// FILE: simple_mt_service.h
// (c) GIE 2011-08-30  05:38
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_MT_SERVICE_2011_08_30_05_38
#define H_GUARD_SIMPLE_MT_SERVICE_2011_08_30_05_38
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "simple_common.hpp"

#include "simple_mt_service_fwd.hpp"
//================================================================================================================================================
namespace gie {

	struct root {};


	template<class Self, template <class> class EfilterLogAndSilence>
	struct simple_asio_mt_service_t
		: EfilterLogAndSilence< Self >
	{

		boost::asio::io_service& service(){ return m_service; }

		void signal_stop(){
			GIE_DEBUG_TRACE();
			this->shut_down_service_();
		}

		void sync_on_stop(){
			GIE_DEBUG_TRACE();
			m_workers.join_all();
		}

		template <class Fun>
		void post(Fun&& fun){

			this->service().post([this, fun]{
				try{
					fun();
				}catch(...){
					this->efilter_log_and_silence_exception_();
				}
			});

		}

		explicit
		simple_asio_mt_service_t(unsigned int num_of_workers=10)
			: m_alive( boost::ref(m_service) )
		{
			assert(num_of_workers);

			for(unsigned int i=0; i<num_of_workers; ++i){
				m_workers.create_thread([this]{ this->worker_main_(); } );
			}
		}

		~simple_asio_mt_service_t(){ // normally this should be term()ed from parent
			try{
				GIE_DEBUG_LOG("dtor in");

				this->shut_down_service_on_scope_exit_();
				m_workers.join_all();

				GIE_DEBUG_LOG("dtor out");
			}catch(...){
				this->efilter_log_and_silence_exception_();
			}

		}

	private:
		boost::asio::io_service m_service;
		boost::optional<boost::asio::io_service::work>   m_alive;
		boost::thread_group   m_workers;

		void worker_main_(){
			try{
				GIE_DEBUG_LOG("async thread started");
				m_service.run();
				GIE_DEBUG_LOG("async thread about to exit");
			}catch(...) {
				this->efilter_log_and_silence_exception_();
			}
		}

		void shut_down_service_(){


			if( m_alive ) {
				GIE_DEBUG_LOG("shutting down");
				m_alive.reset();
			} else {
				GIE_DEBUG_LOG("already shut down");
			}
		}

		void shut_down_service_on_scope_exit_(){
			this->shut_down_service_();
		}

	};


	template<class Self, class IOHub>
	struct simple_asio_mt_service_proxy_t {

		typedef IOHub io_hub_t;

		simple_asio_mt_service_proxy_t( std::shared_ptr<io_hub_t> const& io_hub) : m_io_hub( io_hub ) {}


		template <class Fun>
		void post(Fun&& fun){
			m_io_hub->post( std::move(fun) );
		}

		boost::asio::io_service& service(){ return m_io_hub->service(); }

		~simple_asio_mt_service_proxy_t(){
			GIE_DEBUG_TRACE_INOUT();
		}

	private:
		std::shared_ptr< io_hub_t > m_io_hub;
	};


}
//================================================================================================================================================
#endif
//================================================================================================================================================
