//================================================================================================================================================
// FILE: ut_next_power_of_two.cpp
// (c) GIE 2011-12-14  19:53
//
//================================================================================================================================================
//#include "stdafx.h"
//================================================================================================================================================
#define BOOST_TEST_MODULE libgie test suite
//================================================================================================================================================
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
#include "gie/pair_ostream.hpp"

#include "gie/next_power_of_two.hpp"
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE( next_power_of_two )
//================================================================================================================================================
BOOST_AUTO_TEST_CASE( generic_get_first_low_high_01 ) {

        {
            auto const value = 0b0u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(0, 0);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b0u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(0, 0);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b01u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(1, 1);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b10u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(2, 2);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b11u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(1, 2);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b100u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(3, 3);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b101u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(1, 3);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b110u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(2, 3);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = 0b111u;
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(1, 3);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

        {
            auto const value = std::numeric_limits<unsigned int>::max();
            auto const expected_result =  std::make_pair<unsigned int, unsigned int>(1, std::numeric_limits<unsigned int>::digits);

            BOOST_TEST(gie::generic_get_first_low_high(value) == expected_result);
            BOOST_TEST(gie::gcc_get_first_low_high(value) == expected_result);
        }

}
//================================================================================================================================================
BOOST_AUTO_TEST_CASE( test1 ) {
        BOOST_TEST(gie::next_power_of_two(0b0000u) == 0b0001u);
        BOOST_TEST(gie::next_power_of_two(0b0001u) == 0b0001u);

        BOOST_TEST(gie::next_power_of_two(0b0010u) == 0b0010u);

        BOOST_TEST(gie::next_power_of_two(0b0011u) == 0b0100u);
        BOOST_TEST(gie::next_power_of_two(0b0100u) == 0b0100u);

        BOOST_TEST(gie::next_power_of_two(0b0101u) == 0b1000u);
        BOOST_TEST(gie::next_power_of_two(0b0110u) == 0b1000u);
        BOOST_TEST(gie::next_power_of_two(0b0111u) == 0b1000u);
}

BOOST_AUTO_TEST_CASE( test2 ) {

        for(unsigned int i = 0; i< 1024; ++i ){
            auto const np = gie::next_power_of_two(i);
            auto const np_generic = gie::generic_next_power_of_two(i);

            BOOST_TEST(np == np_generic);
        }
}


BOOST_AUTO_TEST_CASE(test3) {

    srand (time(NULL));

    for(unsigned int i = 0; i< 4*1024; ++i ){
        unsigned int const v =rand();
        auto const np = gie::next_power_of_two(v);
        auto const np_generic = gie::generic_next_power_of_two(v);

        BOOST_TEST(np == np_generic);
    }
}


BOOST_AUTO_TEST_CASE(test4) {

    unsigned int v = std::numeric_limits<unsigned int>::max();

    BOOST_TEST(gie::next_power_of_two_as_exponent(v) == std::numeric_limits<unsigned int>::digits);
    BOOST_TEST(gie::gcc_next_power_of_two_as_exponent(v) == std::numeric_limits<unsigned int>::digits);
    BOOST_TEST(gie::generic_next_power_of_two_as_exponent(v) == std::numeric_limits<unsigned int>::digits);

    BOOST_CHECK_THROW(gie::next_power_of_two(v), gie::exception::overflow_error);
    BOOST_CHECK_THROW(gie::gcc_next_power_of_two(v), gie::exception::overflow_error);
    BOOST_CHECK_THROW(gie::generic_next_power_of_two(v), gie::exception::overflow_error);


}

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================

