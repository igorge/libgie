//================================================================================================================================================
// FILE: ut_sio2_test03.cpp
// (c) GIE 2016-11-04  18:08
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
    BOOST_AUTO_TEST_CASE( test_integer )
    {

        typedef std::vector<unsigned char> container_t;

        container_t container;

        {
            sio2::push_back_writer_t <container_t> os{container};


            int v = -1;
            os(sio2::as<sio2::tag::int32_le>(v));

            BOOST_TEST( container.size() == 4);
            BOOST_TEST( container.at(0) == 0xff);
            BOOST_TEST( container.at(1) == 0xff);
            BOOST_TEST( container.at(2) == 0xff);
            BOOST_TEST( container.at(3) == 0xff);
        }

        {
            sio2::range_reader_t<container_t> is{container};

            int v = 0;
            is(sio2::as<sio2::tag::int32_le>(v));

            BOOST_TEST( v == -1);
        }

    }


    BOOST_AUTO_TEST_CASE( test_integer2 )
    {

        typedef std::vector<unsigned char> container_t;

        container_t container;

        {
            sio2::push_back_writer_t <container_t> os{container};


            long v = -1;
            os(sio2::as<sio2::tag::int32_le>(v));

            BOOST_TEST( container.size() == 4);
            BOOST_TEST( container.at(0) == 0xff);
            BOOST_TEST( container.at(1) == 0xff);
            BOOST_TEST( container.at(2) == 0xff);
            BOOST_TEST( container.at(3) == 0xff);
        }

        {
            sio2::range_reader_t<container_t> is{container};

            long v = 0;
            is(sio2::as<sio2::tag::int32_le>(v));

            BOOST_TEST( v == -1);
        }

    }


    BOOST_AUTO_TEST_CASE( test_integer3 )
    {

        typedef std::vector<unsigned char> container_t;

        for(unsigned int i = 0; i< 1024; ++i){

            container_t container;

            int const v_const = rand() - rand();

            {
                sio2::push_back_writer_t<container_t> os{container};


                int v = v_const;
                os(sio2::as<sio2::tag::int32_le>(v));

                BOOST_TEST(container.size() == 4);
            }

            {
                sio2::range_reader_t<container_t> is{container};

                int v = 0;
                is(sio2::as<sio2::tag::int32_le>(v));

                BOOST_TEST(v == v_const);
            }
        }

    }




    BOOST_AUTO_TEST_CASE( test_integer4 )
    {

        typedef std::vector<unsigned char> container_t;

        for(unsigned int i = 0; i< 1024; ++i){

            container_t container;

            std::int64_t const v_const = rand() - rand();

            {
                sio2::push_back_writer_t<container_t> os{container};


                std::int64_t v = v_const;
                os(sio2::as<sio2::tag::int32_le>(v));

                BOOST_TEST(container.size() == 4);
            }

            {
                sio2::range_reader_t<container_t> is{container};

                std::int64_t v = 0;
                is(sio2::as<sio2::tag::int32_le>(v));

                BOOST_TEST(v == v_const);
            }
        }

    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
