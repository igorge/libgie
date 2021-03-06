//================================================================================================================================================
// FILE: ut_sio2_test04.cpp
// (c) GIE 2016-11-05  04:50
//
//================================================================================================================================================
//================================================================================================================================================
#include "gie/sio2/sio2_range_reader.hpp"
#include "gie/sio2/sio2_push_back_stream.hpp"
#include "gie/sio2/sio2_core.hpp"
#include "gie/exceptions.hpp"

#include <boost/test/unit_test.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE( sio2_test_suite03 )
//================================================================================================================================================
    using namespace gie;
//================================================================================================================================================

    BOOST_AUTO_TEST_CASE( test_float_01 ) {

        typedef std::vector<unsigned char> container_t;

        float const v_const = 3.1415926f;

        container_t container;

        {
            sio2::push_back_writer_t <container_t> os{container};

            float v = v_const;

            os(sio2::as<sio2::tag::float32_le>(v));
        }

        {
            sio2::range_reader_t<container_t> is{container};

            float v = 0;
            is(sio2::as<sio2::tag::float32_le>(v));

            BOOST_TEST(v==v_const);


        }


    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
