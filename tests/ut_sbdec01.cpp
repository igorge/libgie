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

        explicit dummy_t(int){
            GIE_DEBUG_LOG("ctor int");
        }

        dummy_t() = delete;

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
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = (sbdec::with_name("constant", sbdec::constant<sio2::tag::octet>(1)) >> sbdec::constant<sio2::tag::octet>(2)).name("parser") >>
                        sbdec::value<sio2::tag::octet>()  >> [](auto&&, auto&&){ return dummy_t{23} ;};

            auto p2 = ref(p) >> sbdec::value<sio2::tag::octet>().name("last octet");



            auto r = p2(reader, nullptr);

            //int* g = r;

        }



}

    BOOST_AUTO_TEST_CASE( test_02 ) {

        typedef std::vector<unsigned char> container_t;
        container_t container = {3, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat<sio2::tag::octet>( sbdec::value<sio2::tag::octet>() );


            auto r = p(reader, nullptr);

            BOOST_TEST(r.size() = 3);
            BOOST_TEST(r.at(0) = 2);
            BOOST_TEST(r.at(1) = 5);
            BOOST_TEST(r.at(2) = 9);

        }



    }

    BOOST_AUTO_TEST_CASE( test_03 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {3};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat<sio2::tag::octet>( [](auto&&, auto&&){ return dummy_t{23} ;} );


            auto r = p(reader, nullptr);

            static_assert( std::is_same<decltype(r)::value_type, dummy_t>::value);
            BOOST_TEST(r.size() = 3);
        }



    }

    BOOST_AUTO_TEST_CASE( test_04 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat_n<4>( [](auto&&, auto&&){ return dummy_t{23} ;} );


            auto r = p(reader, nullptr);

            static_assert( std::is_same<decltype(r)::value_type, dummy_t>::value);
            BOOST_TEST(r.size() = 4);
        }



    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
