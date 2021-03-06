//================================================================================================================================================
// FILE: ut_caching_istream_iterator.cpp
// (c) GIE 2016-10-21  05:05
//
//================================================================================================================================================
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
#include "gie/caching_istream_iterator.hpp"
#include "gie/simple_caching_allocator.hpp"
#include "gie/simple_to_std_allocator.hpp"

#include <boost/iostreams/stream.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE(caching_istream_iterator_tests )
//================================================================================================================================================

    using allocator_t = gie::simple_to_std_allocator_t<void, gie::simple_caching_allocator>;
    gie::simple_caching_allocator simple_alloc{4,13};

//    using allocator_t = std::allocator<void>;
//    allocator_t simple_alloc{};
    
    
    
    
    BOOST_AUTO_TEST_CASE( test01 ) {

        std::vector<char> data={0,1, 2,3, 4,5, 6};

        boost::iostreams::basic_array_source<char> source(data.data(),data.size());
        boost::iostreams::stream<boost::iostreams::basic_array_source <char> > input_stream(source);

        BOOST_TEST( input_stream.good() );

        gie::impl::caching_istream_iterator_shared_t<allocator_t> imp1{ allocator_t{simple_alloc}, input_stream, 2, 2};

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

        gie::impl::caching_istream_iterator_shared_t<allocator_t> imp1{allocator_t{simple_alloc}, input_stream, 2, 2};


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




    BOOST_AUTO_TEST_CASE( test03 ) {

        std::istringstream input_stream{""};

        BOOST_TEST( input_stream.good() );

        auto r = gie::make_istream_range( allocator_t{simple_alloc}, input_stream, 2, 10 );

        GIE_CHECK(r.end() == r.end());
        GIE_CHECK( r.begin()==r.end() );

    }

    BOOST_AUTO_TEST_CASE( test04 ) {

        std::vector<char> data={0};

        boost::iostreams::basic_array<char> source(data.data(),data.size());
        boost::iostreams::stream<boost::iostreams::basic_array<char> > input_stream(source);

        BOOST_TEST( input_stream.good() );

        auto r = gie::make_istream_range(allocator_t{simple_alloc}, input_stream,  2, 10  );

        BOOST_CHECK(r.end() == r.end());
        BOOST_CHECK( r.begin()!=r.end() );

        auto i = r.begin();
        BOOST_CHECK(i==r.begin());

        ++i;
        BOOST_CHECK(i!=r.begin());
        BOOST_CHECK(i==r.end());
    }


    BOOST_AUTO_TEST_CASE( test05 ) {

        std::vector<char> data = {42};

        boost::iostreams::basic_array<char> source(data.data(), data.size());
        boost::iostreams::stream<boost::iostreams::basic_array<char> > input_stream(source);

        BOOST_TEST(input_stream.good());

        auto r = gie::make_istream_range(allocator_t{simple_alloc}, input_stream, 2, 10);

        BOOST_CHECK(r.end() == r.end());
        BOOST_CHECK(r.begin() != r.end());
        BOOST_CHECK(r.end() != r.begin());

        auto i = r.begin();
        BOOST_CHECK(i == r.begin());

        BOOST_TEST(*i == 42);

        auto b = i;

        BOOST_CHECK(b == i);

        ++i;
        BOOST_CHECK(i != r.begin());
        BOOST_CHECK(i == r.end());
        BOOST_CHECK(r.end() == i);

    }


    BOOST_AUTO_TEST_CASE( test06 ) {

        std::vector<char> data = {42,43,3};

        boost::iostreams::basic_array<char> source(data.data(), data.size());
        boost::iostreams::stream<boost::iostreams::basic_array<char> > input_stream(source);

        BOOST_TEST(input_stream.good());

        auto r = gie::make_istream_range<allocator_t>(allocator_t{simple_alloc}, input_stream, 2, 10);

        BOOST_CHECK(r.end() == r.end());
        BOOST_CHECK(r.begin() != r.end());
        BOOST_CHECK(r.end() != r.begin());

        auto i = r.begin();

        BOOST_CHECK(i == r.begin());


        BOOST_CHECK(i != r.end());
        BOOST_TEST(*i == 42);
        ++i;
        BOOST_CHECK(i != r.end());


        BOOST_CHECK(i != r.end());
        BOOST_TEST(*i == 43);
        ++i;
        BOOST_CHECK(i != r.end());


        BOOST_CHECK(i != r.end());
        BOOST_TEST(*i == 3);
        ++i;
        BOOST_CHECK(i == r.end());

    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================

