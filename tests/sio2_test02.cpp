//================================================================================================================================================
// FILE: sio2_test02.cpp
// (c) GIE 2016-10-11
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
BOOST_AUTO_TEST_SUITE( sio2_test_suite02 )
//================================================================================================================================================
using namespace gie;

namespace test {

    struct sempty_t {
        bool touched_ = false;

    };

    template <class StreamT>
    void serialize(sempty_t& v, StreamT & stream){
        v.touched_ = true;
    }

}


BOOST_AUTO_TEST_CASE( sio2_empty_struct )
{
    typedef std::vector<unsigned char> container_t;

    container_t container;

    sio2::push_back_writer_t <container_t> os{container};


    test::sempty_t s1{};

    os(s1);

    BOOST_CHECK( container.empty() );
    BOOST_CHECK( s1.touched_ );
}


BOOST_AUTO_TEST_CASE( sio2_empty_struct2 )
{
    typedef std::vector<unsigned char> container_t;

    container_t container;

    sio2::range_reader_t<container_t> is{container};


    test::sempty_t s1{};

    is(s1);

    BOOST_CHECK( container.empty() );
    BOOST_CHECK( s1.touched_ );
}

//================================================================================================================================================

namespace test2 {


    struct s_t {
        std::uint_fast8_t u8_v;
    };
    template <class StreamT>
    void serialize(s_t& v, StreamT & stream){
        stream (sio2::as<sio2::tag::octet>(v.u8_v));
    }

    struct s1_t {
        std::uint_fast8_t u8_v;
        std::uint_fast32_t u32_v;
        std::uint_fast32_t u8_as_32_v;
    };
    template <class StreamT>
    void serialize(s1_t& v, StreamT & stream){
        stream (sio2::as<sio2::tag::octet>(v.u8_v))
               (sio2::as<sio2::tag::uint32_le>(v.u32_v))
               (sio2::as<sio2::tag::octet>(v.u8_as_32_v));
    }


    struct s2_t {
        std::uint_fast8_t u8_v;
        s1_t s1_v;
    };
    template <class StreamT>
    void serialize(s2_t& v, StreamT & stream){
        stream (sio2::as<sio2::tag::octet>(v.u8_v))
               (v.s1_v);
    }

}


BOOST_AUTO_TEST_CASE( sio2_array_read_write_01 )
{

    typedef std::vector<unsigned char> container_t;

    container_t container;

    {
        sio2::push_back_writer_t<container_t> os{container};

        std::array<test2::s_t,3> s{3,2,1};

        os(s);

        BOOST_TEST( container.size() == 3);
        BOOST_TEST( container.at(0) == 3);
        BOOST_TEST( container.at(1) == 2);
        BOOST_TEST( container.at(2) == 1);

    }

    {
        sio2::range_reader_t<container_t> is{container};

        std::array<test2::s_t,3> s{0,0,0};
        is(s);

        BOOST_TEST(s[0].u8_v==3);
        BOOST_TEST(s[1].u8_v==2);
        BOOST_TEST(s[2].u8_v==1);
    }

}



BOOST_AUTO_TEST_CASE( sio2_array_as_read_write_02 )
{

    typedef std::vector<unsigned char> container_t;

    container_t container;

    {
        sio2::push_back_writer_t <container_t> os{container};

        std::array<unsigned int, 3> s{3, 2, 1};

        os(sio2::as_array_of<sio2::tag::octet>(s));

        BOOST_TEST( container.size() == 3);
        BOOST_TEST( container.at(0) == 3);
        BOOST_TEST( container.at(1) == 2);
        BOOST_TEST( container.at(2) == 1);

    }

    {
        sio2::range_reader_t <container_t> is{container};

        std::array<unsigned int, 3> s{0, 0, 0};

        is(sio2::as_array_of<sio2::tag::octet>(s));

        BOOST_TEST(s[0]==3);
        BOOST_TEST(s[1]==2);
        BOOST_TEST(s[2]==1);
    }
}



BOOST_AUTO_TEST_CASE( sio2_c_array_read_write_01 )
{

    typedef std::vector<unsigned char> container_t;

    container_t container;

    {
        sio2::push_back_writer_t<container_t> os{container};

        test2::s_t s[3] = {3,2,1};

        os(s);

        BOOST_TEST( container.size() == 3);
        BOOST_TEST( container.at(0) == 3);
        BOOST_TEST( container.at(1) == 2);
        BOOST_TEST( container.at(2) == 1);
    }

    {
        sio2::range_reader_t<container_t> is{container};

        test2::s_t s[3] = {0,0,0};
        is(s);

        BOOST_TEST(s[0].u8_v==3);
        BOOST_TEST(s[1].u8_v==2);
        BOOST_TEST(s[2].u8_v==1);
    }

}


BOOST_AUTO_TEST_CASE( sio2_c_array_as_read_write_02 )
{

    typedef std::vector<unsigned char> container_t;

    container_t container;

    {
        sio2::push_back_writer_t <container_t> os{container};

        unsigned int s[3] = {3, 2, 1};

        os(sio2::as_array_of<sio2::tag::octet>(s));

        BOOST_TEST( container.size() == 3);
        BOOST_TEST( container.at(0) == 3);
        BOOST_TEST( container.at(1) == 2);
        BOOST_TEST( container.at(2) == 1);

    }

    {
        sio2::range_reader_t <container_t> is{container};

        unsigned int s[3] = {3, 2, 1};

        is(sio2::as_array_of<sio2::tag::octet>(s));

        BOOST_TEST(s[0]==3);
        BOOST_TEST(s[1]==2);
        BOOST_TEST(s[2]==1);
    }
}




    BOOST_AUTO_TEST_CASE( sio2_struct_01 )
{
    typedef std::vector<std::uint8_t> container_t;

    container_t container={255};

    sio2::range_reader_t<container_t> is{container};


    test2::s_t s{0};

    BOOST_CHECK( s.u8_v==0 );

    is(s);

    BOOST_CHECK( s.u8_v==255 );
    BOOST_CHECK_THROW(is.read(), sio2::exception::underflow);
}



BOOST_AUTO_TEST_CASE( sio2_struct_02 )
{
    typedef std::vector<unsigned char> container_t;

    container_t container;

    sio2::push_back_writer_t<container_t> os{container};


    test2::s2_t s;

    s.u8_v = 54;
    s.s1_v.u8_v = 124;
    s.s1_v.u32_v = 0xfafbfcfd;
    s.s1_v.u8_as_32_v = 22;

    os(s);

    BOOST_CHECK( container.size() == 7 );

    BOOST_CHECK( container.at(0) == 54 );
    BOOST_CHECK( container.at(1) == 124 );

    BOOST_CHECK( container.at(2) == 0xfd );
    BOOST_CHECK( container.at(3) == 0xfc );
    BOOST_CHECK( container.at(4) == 0xfb );
    BOOST_CHECK( container.at(5) == 0xfa);

    BOOST_CHECK( container.at(6) == 22 );


    test2::s2_t s2;

    s2.u8_v = -1;
    s2.s1_v.u8_v = -1;
    s2.s1_v.u32_v = -1;
    s2.s1_v.u8_as_32_v = -1;

    sio2::range_reader_t<container_t> is{container};
    is(s2);

    BOOST_CHECK(s2.u8_v == 54);
    BOOST_CHECK(s2.s1_v.u8_v == 124);
    BOOST_CHECK(s2.s1_v.u32_v == 0xfafbfcfd);
    BOOST_CHECK(s2.s1_v.u8_as_32_v == 22);

}



//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
