//================================================================================================================================================
// FILE: asio_simple_service.h
// (c) GIE 2011-08-19  04:33
//
//================================================================================================================================================
#ifndef H_GUARD_ASIO_SIMPLE_SERVICE_2011_08_19_04_33
#define H_GUARD_ASIO_SIMPLE_SERVICE_2011_08_19_04_33
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "simple_common.hpp"

#include "gie/util-scope-exit.hpp"
//================================================================================================================================================
namespace gie {

	template< template <class> class EfilterLogAndSilence = default_efilter_log_and_silence_exception>
	struct simple_asio_service_t
		: EfilterLogAndSilence< simple_asio_service_t<EfilterLogAndSilence> >
	{

		boost::asio::io_service& service()const{
            assert(m_service != nullptr);
			return *m_service;
		}

        shared_io_service_t shared_service()const{
            return m_service;
        }

		void stop_sync(){
			GIE_DEBUG_TRACE_INOUT();
			this->shut_down_service_();
            m_main_worker.join();
		}

		template <class Fun>
		void post(Fun&& fun){
			this->service().post(std::forward<Fun>(fun));
		}

		template <class T>
        explicit simple_asio_service_t(T&& io)
			: m_service( std::forward<T>(io) )
            , m_alive( boost::ref(service()) )
        	, m_main_worker( [this]()->void{ worker_main_(); } )
		{
		}

		~simple_asio_service_t(){
            try{
            	GIE_DEBUG_TRACE_INOUT();

                this->shut_down_service_on_scope_exit_();
                m_main_worker.join();

            }catch(...){
            	this->efilter_log_and_silence_exception_();
            }

		}

	private:
		shared_io_service_t m_service;
        boost::optional<boost::asio::io_service::work>   m_alive;
        boost::thread   m_main_worker;

        void worker_main_(){
        	GIE_DEBUG_LOG("async thread started");
            GIE_DEBUG_SCOPE_EXIT_LOG("async thread about to exit");

            for(;;){
                try {
                    service().run();
                    return;
                } catch(...){
                    this->efilter_log_and_silence_exception_();
                }
            }

        }

        void shut_down_service_(){
			GIE_DEBUG_TRACE_INOUT();

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


} // ns gie

//================================================================================================================================================
#endif
//================================================================================================================================================
