//================================================================================================================================================
// FILE: sio2_test.cpp
// (c) GIE 2016-10-11  19:55
//
//================================================================================================================================================
//#include "stdafx.h"
//================================================================================================================================================
#include "gie/sio2/sio2_range_reader.hpp"
#include "gie/sio2/sio2_push_back_stream.hpp"
#include "gie/sio2/sio2_core.hpp"
#include "gie/exceptions.hpp"

#include <boost/test/unit_test.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE( sio2_test_suite )
//================================================================================================================================================
using namespace gie;


BOOST_AUTO_TEST_CASE( sio_as_t_test_as_value )
{
    auto const magic = 1024;

    unsigned int data = magic;
    auto const v= sio2::as<sio2::tag::octet>(data);

    BOOST_CHECK(v.value == magic);

    v.value = 23;
    BOOST_CHECK( data == 23);

}

BOOST_AUTO_TEST_CASE( test_unsigned_fit_signed )
{
    unsigned char const v1 = std::numeric_limits<unsigned char>::max();
    signed char const v2 = v1;
    BOOST_CHECK( static_cast<int>(v2)== -1 ); //not guaranteed by c++ standard
}



BOOST_AUTO_TEST_CASE( test_push_back_writer )
{
    typedef std::vector<signed char> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};
    os.write(static_cast<unsigned char>(std::numeric_limits<unsigned char>::max()));

    BOOST_CHECK( container.at(0) == -1);
}


BOOST_AUTO_TEST_CASE( test_push_back_writer2 )
{
    typedef std::vector<int> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};
    os.write(static_cast<unsigned char>(std::numeric_limits<unsigned char>::max()));

    BOOST_CHECK( container.at(0) == std::numeric_limits<unsigned char>::max() );

}

BOOST_AUTO_TEST_CASE( test_push_back_writer3 )
{
    typedef std::vector<unsigned short> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};
    os.write(static_cast<unsigned int>(-1));

    BOOST_CHECK( container.at(0) == std::numeric_limits<unsigned char>::max() );

}


BOOST_AUTO_TEST_CASE( test_push_back_writer4 )
{
    typedef std::vector<char> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};

    unsigned char v = 1;
    unsigned char v2 = std::numeric_limits<unsigned char>::max() ;

    os(sio2::as<sio2::tag::octet>(v))(sio2::as<sio2::tag::octet>(v2));


    BOOST_CHECK( container.at(0) == 1 );
    BOOST_CHECK( container.at(1) == -1 );
}


BOOST_AUTO_TEST_CASE( test_push_back_writer5 )
{
    typedef std::vector<char> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};

    unsigned int v = 1;

    os(sio2::as<sio2::tag::uint32_le>(v));

    BOOST_CHECK( container.size() == 4 );
    BOOST_CHECK( container.at(0) == 1 );
    BOOST_CHECK( container.at(1) == 0 );
    BOOST_CHECK( container.at(2) == 0 );
    BOOST_CHECK( container.at(3) == 0 );

}


BOOST_AUTO_TEST_CASE( test_push_back_writer6 )
{
    typedef std::vector<unsigned char> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};

    unsigned int v = 0xfafbfcfd;

    os(sio2::as<sio2::tag::uint32_le>(v));

    BOOST_CHECK( container.size() == 4 );
    BOOST_CHECK( container.at(0) == 0xfd );
    BOOST_CHECK( container.at(1) == 0xfc );
    BOOST_CHECK( container.at(2) == 0xfb );
    BOOST_CHECK( container.at(3) == 0xfa );

}


BOOST_AUTO_TEST_CASE( test_push_back_writer_uint64_t )
{
    typedef std::vector<unsigned char> container_t;

    container_t container;
    sio2::push_back_writer_t <container_t> os{container};

    std::uint64_t v = 0xfafbfcfd;

    os(sio2::as<sio2::tag::uint32_le>(v));

    BOOST_CHECK( container.size() == 4 );
    BOOST_CHECK( container.at(0) == 0xfd );
    BOOST_CHECK( container.at(1) == 0xfc );
    BOOST_CHECK( container.at(2) == 0xfb );
    BOOST_CHECK( container.at(3) == 0xfa );

}



BOOST_AUTO_TEST_CASE( test_range_reader_t_empty )
{
    typedef std::vector<signed char> container_t;

    container_t container;
    sio2::range_reader_t<container_t> is{container};

    BOOST_CHECK_THROW(is.read(), sio2::exception::underflow);

}

BOOST_AUTO_TEST_CASE( test_range_reader_t )
{
    typedef std::vector<signed char> container_t;

    container_t container = {-1, 1};
    sio2::range_reader_t<container_t> is{container};

    auto const v1 = is.read();
    BOOST_CHECK(v1 == std::numeric_limits<unsigned char>::max());

    auto const v2 = is.read();
    BOOST_CHECK(v2 == 1);

    BOOST_CHECK_THROW(is.read(), sio2::exception::underflow);

}


BOOST_AUTO_TEST_CASE( test_range_reader_t_int_base )
{
    typedef std::vector<int> container_t;

    container_t container = {-1, 1};
    sio2::range_reader_t<container_t> is{container};

    auto const v1 = is.read();
    BOOST_CHECK(v1 == std::numeric_limits<unsigned char>::max());

    auto const v2 = is.read();
    BOOST_CHECK(v2 == 1);

    BOOST_CHECK_THROW(is.read(), sio2::exception::underflow);

}




BOOST_AUTO_TEST_CASE( test_range_reader_t_uint32_le )
{
    typedef std::vector<unsigned char> container_t;

    container_t container={0xfd, 0xfc, 0xfb, 0xfa};
    sio2::range_reader_t<container_t> is{container};

    unsigned int v = 0;

    is(sio2::as<sio2::tag::uint32_le>(v));

    BOOST_CHECK(v == 0xfafbfcfd);
    BOOST_CHECK_THROW(is.read(), sio2::exception::underflow);
}

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
