//================================================================================================================================================
// FILE: safe_main.h
// (c) GIE 2011-07-11  15:18
//
//================================================================================================================================================
#ifndef H_GUARD_SAFE_MAIN_2011_07_11_15_18
#define H_GUARD_SAFE_MAIN_2011_07_11_15_18
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/log/log.hpp"

#include <boost/exception/diagnostic_information.hpp>
//================================================================================================================================================
namespace gie {
    
    template <class MainFun>
    int main(MainFun const& fun){
        try {
            
            fun();
            
        } catch( boost::exception const & e ) {
            auto const einfo = boost::get_error_info<gie::exception::error_code_einfo>(e);

            if(einfo){
                GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e) << "\n" << "error code message: " << einfo->message() );
            } else {
                GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e));
            }
            return EXIT_FAILURE;
        } catch( std::exception const & e ) {
            GIE_LOG( "\n======= uncaught exception =======\n" << typeid(e).name() << "\n" << e.what() );
            return EXIT_FAILURE;
        } /*catch( ... ) {
            GIE_LOG( "\n======= unknown uncaught exception =======" );
            return EXIT_FAILURE;
        } */   
        
        return EXIT_SUCCESS;
    }
    
}
//================================================================================================================================================
#endif
//================================================================================================================================================
