//================================================================================================================================================
// FILE: ut_sbdec01.cpp
// (c) GIE 2016-11-08  18:04
//
//================================================================================================================================================
#include "gie/sbdec/sbdec.hpp"
#include "gie/sio2/sio2_range_reader.hpp"

#include <boost/test/unit_test.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE( sbdec01 )
//================================================================================================================================================
using namespace gie;
//================================================================================================================================================

    struct dummy_t {

        dummy_t(){
            GIE_DEBUG_LOG("ctor()");
        }

        dummy_t(dummy_t&&){
            GIE_DEBUG_LOG("move ctor");
        }

        dummy_t(dummy_t const&) = delete;

        dummy_t& operator=(dummy_t const&) = delete;

        dummy_t& operator=(dummy_t &&){
            GIE_DEBUG_LOG("move=");
            return *this;
        }

        ~dummy_t(){
            GIE_DEBUG_LOG("dtor()");
        }

    };

BOOST_AUTO_TEST_CASE( test_01 ) {

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto p = (sbdec::with_name("constant", sbdec::constant<sio2::tag::octet>(1)) >> sbdec::constant<sio2::tag::octet>(2)).name("parser") >>
                        sbdec::value<sio2::tag::octet>()  >> [](auto&&, auto&&){ return dummy_t{} ;};




            auto r = p(reader, nullptr);

            //int* g = r;

        }



}

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
