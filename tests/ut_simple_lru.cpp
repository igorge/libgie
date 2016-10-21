//================================================================================================================================================
// FILE: ut_simple_lru.cpp
// (c) GIE 2016-10-20  17:10
//
//================================================================================================================================================
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
#include "gie/simple_lru.hpp"

#include "gie/simple_to_std_allocator.hpp"
#include "gie/simple_caching_allocator.hpp"


#include <boost/optional/optional_io.hpp>
//================================================================================================================================================
BOOST_AUTO_TEST_SUITE(simple_lru_tests )
//================================================================================================================================================

//    template <class T> using allocator_t = gie::simple_to_std_allocator_t<T, gie::simple_caching_allocator>;
//    gie::simple_caching_allocator simple_alloc{4,13};

    template <class T> using allocator_t = std::allocator<T>;
    allocator_t<void> simple_alloc{};

    BOOST_AUTO_TEST_CASE(test1) {

        typedef gie::lru_t<unsigned int, unsigned int, allocator_t> lru1_t;
        lru1_t lru{3, allocator_t<char>{simple_alloc}};

        auto const& r = lru.insert(0,1);

        BOOST_TEST(r==1);

        auto const& r2 = lru.find(0);
        BOOST_TEST( r2 );
        BOOST_TEST( &(*r2)==&r);

        BOOST_TEST( !lru.find(1) );

        std::vector<lru1_t::value_type> vec;
        for( auto && v: lru.get_lru_list()){vec.emplace_back(v);}

        BOOST_TEST( vec.size() == 1);
        BOOST_TEST( vec.at(0).key == 0);
        BOOST_TEST( vec.at(0).value==1);

        auto const& r3 = lru.insert(0,11);
        BOOST_TEST( r3 );
        BOOST_TEST(&r3==&r);

        vec.clear();
        for( auto && v: lru.get_lru_list()){vec.emplace_back(v);}

        BOOST_TEST( vec.size() == 1);
        BOOST_TEST( vec.at(0).key == 0);
        BOOST_TEST( vec.at(0).value==11);


    }


    BOOST_AUTO_TEST_CASE(test2) {

        typedef gie::lru_t<unsigned int, unsigned int, allocator_t> lru1_t;
        lru1_t lru{3, allocator_t<char>{simple_alloc}};

        lru.insert(0,1);
        lru.insert(1,3);

        {
            std::vector<lru1_t::value_type> vec;
            for( auto && v: lru.get_lru_list()){
                vec.emplace_back(v);
            }

            BOOST_TEST( vec.size() == 2);

            BOOST_TEST( vec.at(0).key == 0);
            BOOST_TEST( vec.at(0).value==1);

            BOOST_TEST( vec.at(1).key == 1);
            BOOST_TEST( vec.at(1).value==3);
        }

        {
            BOOST_TEST( lru.find(0) );

            std::vector<lru1_t::value_type> vec;
            for( auto && v: lru.get_lru_list()){
                vec.emplace_back(v);
            }

            BOOST_TEST( vec.size() == 2);

            BOOST_TEST( vec.at(1).key == 0);
            BOOST_TEST( vec.at(1).value==1);

            BOOST_TEST( vec.at(0).key == 1);
            BOOST_TEST( vec.at(0).value==3);
        }

        {
            lru.insert(1,42);

            std::vector<lru1_t::value_type> vec;
            for( auto && v: lru.get_lru_list()){
                vec.emplace_back(v);
            }

            BOOST_TEST( vec.size() == 2);

            BOOST_TEST( vec.at(1).key == 1);
            BOOST_TEST( vec.at(1).value==42);

            BOOST_TEST( vec.at(0).key == 0);
            BOOST_TEST( vec.at(0).value==1);
        }


    }


    BOOST_AUTO_TEST_CASE(test3) {

        typedef gie::lru_t<unsigned int, unsigned int, allocator_t> lru1_t;
        lru1_t lru{3, allocator_t<char>{simple_alloc}};

        lru.insert(0,1);
        lru.insert(1,3);
        lru.insert(2,4);

        {
            std::vector<lru1_t::value_type> vec;
            for( auto && v: lru.get_lru_list()){
                vec.emplace_back(v);
            }

            BOOST_TEST( vec.size() == 3);

            BOOST_TEST( vec.at(0).key == 0);
            BOOST_TEST( vec.at(0).value==1);

            BOOST_TEST( vec.at(1).key == 1);
            BOOST_TEST( vec.at(1).value==3);

            BOOST_TEST( vec.at(2).key == 2);
            BOOST_TEST( vec.at(2).value==4);
        }


        {

            lru.insert(22,76);

            std::vector<lru1_t::value_type> vec;
            for( auto && v: lru.get_lru_list()){
                vec.emplace_back(v);
            }

            BOOST_TEST( vec.size() == 3);


            BOOST_TEST( vec.at(0).key == 1);
            BOOST_TEST( vec.at(0).value==3);

            BOOST_TEST( vec.at(1).key == 2);
            BOOST_TEST( vec.at(1).value==4);

            BOOST_TEST( vec.at(2).key == 22);
            BOOST_TEST( vec.at(2).value==76);
        }


    }

    BOOST_AUTO_TEST_CASE(test4) {

        typedef gie::lru_t<unsigned int, unsigned int, allocator_t> lru1_t;
        lru1_t lru{3, allocator_t<char>{simple_alloc}};

        for(unsigned int i=0; i<1024; ++i){
            lru.insert(i,i);
        }


    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
