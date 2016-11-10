//================================================================================================================================================
// FILE: ut_sbdec01.cpp
// (c) GIE 2016-11-08  18:04
//
//================================================================================================================================================
#include "gie/sbdec/sbdec.hpp"
#include "gie/sio2/sio2_range_reader.hpp"

#include <boost/fusion/tuple.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE( sbdec01 )
//================================================================================================================================================
    using namespace gie;
    using namespace boost::hana::literals;
//================================================================================================================================================

    struct dummy_t {

        explicit dummy_t(int v) : value(v) {
            GIE_DEBUG_LOG("ctor int");
        }

        int value;

        dummy_t() = delete;

        dummy_t(dummy_t&&other) : dummy_t(other.value) {
            GIE_DEBUG_LOG("move ctor");
        }

        dummy_t(dummy_t const&) = delete;

        dummy_t& operator=(dummy_t const&) = delete;

        dummy_t& operator=(dummy_t && other){
            GIE_DEBUG_LOG("move=");
            value = other.value;
            return *this;
        }

        ~dummy_t(){
            GIE_DEBUG_LOG("dtor()");
        }

    };

    auto const dummy_p = [](auto value){
        return sbdec::parser( [value](auto&&, auto&&){ return dummy_t{value};} );
    };


    BOOST_AUTO_TEST_CASE( test_01 ) {

        GIE_DEBUG_LOG("---MARK---");

            typedef std::vector<unsigned char> container_t;
            container_t container = {1, 2, 5, 9};

            {
                sio2::range_reader_t<container_t> reader{container};

                auto const parser = sbdec::value<sio2::tag::octet>();

                auto r = parser(reader, nullptr);

                BOOST_TEST(r[0_c] == 1);

                r = parser(reader, nullptr);

                BOOST_TEST(r[0_c] == 2);
            }
    }

    BOOST_AUTO_TEST_CASE( test_02 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const parser = sbdec::value<sio2::tag::octet>() >> sbdec::value<sio2::tag::octet>();

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c] == 1 );
            BOOST_TEST( r[1_c] == 2 );
        }
    }


    BOOST_AUTO_TEST_CASE( test_02_02 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const parser = sbdec::value<sio2::tag::octet>() >> sbdec::value<sio2::tag::octet>() >> sbdec::value<sio2::tag::octet>();

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c] == 1 );
            BOOST_TEST( r[1_c] == 2 );
            BOOST_TEST( r[2_c] == 5 );
        }
    }

    BOOST_AUTO_TEST_CASE( test_02_03 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const parser = sbdec::value<sio2::tag::octet>() >> sbdec::value<sio2::tag::octet>() >> sbdec::constant<sio2::tag::octet>(5) >> sbdec::value<sio2::tag::octet>();

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c] == 1 );
            BOOST_TEST( r[1_c] == 2 );
            BOOST_TEST( r[2_c] == 9 );
        }
    }

    BOOST_AUTO_TEST_CASE( test_02_04 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const parser = sbdec::value<sio2::tag::octet>() >>
                    (sbdec::value<sio2::tag::octet>() >> sbdec::constant<sio2::tag::octet>(5) >> sbdec::value<sio2::tag::octet>()).as<std::tuple>();

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c] == 1 );
            BOOST_TEST( std::get<0>(r[1_c]) == 2 );
            BOOST_TEST( std::get<1>(r[1_c]) == 9 );
        }
    }

    BOOST_AUTO_TEST_CASE( test_03 ) {

        GIE_DEBUG_LOG("---MARK---");


        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto dummy_p = [](auto value){
                return sbdec::parser( [value](auto&&, auto&&){ return dummy_t{value};} );
            };

            auto const parser = dummy_p(9) >> dummy_p(7);

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c].value == 9 );
            BOOST_TEST( r[1_c].value == 7 );
        }
    }


    BOOST_AUTO_TEST_CASE( test_03_02 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {1, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const parser = dummy_p(9)
                    >> (dummy_p(7) >> dummy_p(10)).as<std::tuple>() >> dummy_p(22);

            auto r = parser(reader, nullptr);

            BOOST_TEST( r[0_c].value == 9 );
            BOOST_TEST( std::get<0>(r[1_c]).value == 7 );
            BOOST_TEST( std::get<1>(r[1_c]).value == 10 );
            BOOST_TEST( r[2_c].value == 22 );
        }
    }



    BOOST_AUTO_TEST_CASE( test_repeat_01 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {3, 2, 5, 9};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat<sio2::tag::octet>( sbdec::value<sio2::tag::octet>() );


            auto rt = p(reader, nullptr);

            auto& r = rt[0_c];

            BOOST_TEST(r.size() = 3);
            BOOST_TEST(r.at(0) = 2);
            BOOST_TEST(r.at(1) = 5);
            BOOST_TEST(r.at(2) = 9);

        }



    }

    BOOST_AUTO_TEST_CASE( test_repeat_01_02 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {3};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat<sio2::tag::octet>( dummy_p(23) );

            auto rt = p(reader, nullptr);
            auto& r = rt[0_c];

            static_assert( std::is_same<std::remove_reference<decltype(r)>::type::value_type, dummy_t>::value);
            BOOST_TEST(r.size() = 3);
        }



    }

    BOOST_AUTO_TEST_CASE( test_repeat_01_03 ) {

        GIE_DEBUG_LOG("---MARK---");

        typedef std::vector<unsigned char> container_t;
        container_t container = {};

        {
            sio2::range_reader_t<container_t> reader{container};

            auto const p = sbdec::repeat_n<4>( dummy_p(21) );

            auto rt = p(reader, nullptr);
            auto& r = rt[0_c];

            static_assert( std::is_same<std::remove_reference<decltype(r)>::type::value_type, dummy_t>::value);
            BOOST_TEST(r.size() = 4);
            BOOST_TEST(r.at(0).value==21);
        }



    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
