//================================================================================================================================================
// FILE: exceptions.cpp
// (c) GIE 2016-10-19  00:27
//
//================================================================================================================================================
#include "gie/exceptions.hpp"
//================================================================================================================================================
namespace gie {

        void e_filter_log_exception(){
            try {
                throw;
            } catch( boost::exception const & e ) {
                auto const einfo = boost::get_error_info<gie::exception::error_code_einfo>(e);

                if(einfo){
                    GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e) << "\n" << "error code message: " << einfo->message() );
                } else {
                    GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e));
                }

                throw ;
            } catch( std::exception const & e ) {
                GIE_LOG( "\n======= uncaught exception =======\n" << typeid(e).name() << "\n" << e.what() );

                throw ;
            } catch( ... ) {
                GIE_LOG( "\n======= unknown uncaught exception =======" );
                throw ;
            }
        }


}
//================================================================================================================================================
