//================================================================================================================================================
// FILE: ut_caching_istream_iterator.cpp
// (c) GIE 2016-10-21  05:05
//
//================================================================================================================================================
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
#include "gie/caching_istream_iterator.hpp"

#include <boost/iostreams/stream.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE(caching_istream_iterator_tests )
//================================================================================================================================================
    BOOST_AUTO_TEST_CASE( test01 ) {

        std::vector<char> data={0,1, 2,3, 4,5, 6};

        boost::iostreams::basic_array_source<char> source(data.data(),data.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source <char> > input_stream(source);

        BOOST_TEST( input_stream.good() );

        gie::impl::caching_istream_iterator_shared_t imp1{input_stream, 2, 2};

        auto r = imp1.get_page(0);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==0);
        BOOST_TEST(r->at(1)==1);

        //get cached page
        r = imp1.get_page(0);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==0);
        BOOST_TEST(r->at(1)==1);

        r = imp1.get_page(1);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==2);
        BOOST_TEST(r->at(1)==3);

        r = imp1.get_page(2);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==4);
        BOOST_TEST(r->at(1)==5);


        //reread evicted page
        r = imp1.get_page(0);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==0);
        BOOST_TEST(r->at(1)==1);


        r = imp1.get_page(4);
        BOOST_TEST(!r);


        r = imp1.get_page(100);
        BOOST_TEST(!r);


        r = imp1.get_page(3);
        BOOST_TEST(r->size()==1);
        BOOST_TEST(r->at(0)==6);

        r = imp1.get_page(1);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==2);
        BOOST_TEST(r->at(1)==3);

        r = imp1.get_page(3);
        BOOST_TEST(r->size()==1);
        BOOST_TEST(r->at(0)==6);

    }

    BOOST_AUTO_TEST_CASE( test02 ) {

        std::vector<char> data={0,1, 2,3, 4,5};

        boost::iostreams::basic_array_source<char> source(data.data(),data.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source <char> > input_stream(source);

        BOOST_TEST( input_stream.good() );

        gie::impl::caching_istream_iterator_shared_t imp1{input_stream, 2, 2};


        auto r = imp1.get_page(2);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==4);
        BOOST_TEST(r->at(1)==5);

        r = imp1.get_page(1);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==2);
        BOOST_TEST(r->at(1)==3);

        r = imp1.get_page(0);
        BOOST_TEST(r->size()==2);
        BOOST_TEST(r->at(0)==0);
        BOOST_TEST(r->at(1)==1);

        r = imp1.get_page(3);
        BOOST_TEST(!r);

    }


//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================

