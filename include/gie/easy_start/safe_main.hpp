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

    namespace impl {

        inline
        int safe_main_exception_filter(){
            try {
                throw;
            } catch( boost::exception const & e ) {
                auto const einfo = boost::get_error_info<gie::exception::error_code_einfo>(e);

                if(einfo){
                    GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e) << "\n" << "error code message: " << einfo->message() );
                } else {
                    GIE_LOG( "\n======= uncaught exception =======\n" << diagnostic_information(e));
                }
            } catch( std::exception const & e ) {
                GIE_LOG( "\n======= uncaught exception =======\n" << typeid(e).name() << "\n" << e.what() );
            } /*catch( ... ) {
                GIE_LOG( "\n======= unknown uncaught exception =======" );
                return EXIT_FAILURE;
            } */

            return EXIT_FAILURE;

        }
    }

    template <class MainFun>
    typename std::enable_if< std::is_same<void, typename std::result_of<MainFun()>::type >::value, int>::type
    main(MainFun const& fun){
        try {
            fun();
        } catch(...){
            return impl::safe_main_exception_filter();
        }
        
        return EXIT_SUCCESS;
    }

    template <class MainFun>
    typename std::enable_if< std::is_integral<typename std::result_of<MainFun()>::type >::value, int>::type
    main(MainFun const& fun){
        try {
            return fun();
        } catch(...){
            return impl::safe_main_exception_filter();
        }
    }

}
//================================================================================================================================================
#endif
//================================================================================================================================================
