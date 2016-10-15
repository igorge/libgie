//================================================================================================================================================
// FILE: ut_simple_caching_allocator.cpp
// (c) GIE 2016-10-15  20:23
//
//================================================================================================================================================
#include <boost/test/unit_test.hpp>
//================================================================================================================================================
#include "gie/simple_caching_allocator_mt_wrapper.hpp"
#include "gie/simple_to_std_allocator.hpp"
#include "gie/simple_caching_allocator.hpp"
#include <boost/shared_ptr.hpp>
//================================================================================================================================================
namespace gie {

    struct boost_test__simple_caching_allocator{
        static void test1(simple_caching_allocator & alloc){

            BOOST_TEST(alloc.effective_exponent_(0)==4);
            BOOST_TEST(alloc.effective_exponent_(1)==4);
            BOOST_TEST(alloc.effective_exponent_(16)==4);
            BOOST_TEST(alloc.effective_exponent_(17)==5);
            BOOST_TEST(alloc.effective_exponent_(8192)==13);
            BOOST_TEST(alloc.effective_exponent_(8193)==-1);

            BOOST_TEST(alloc.bucket_idx_from_size_(0)==0);
            BOOST_TEST(alloc.bucket_idx_from_size_(1)==0);
            BOOST_TEST(alloc.bucket_idx_from_size_(16)==0);
            BOOST_TEST(alloc.bucket_idx_from_size_(17)==1);
            BOOST_TEST(alloc.bucket_idx_from_size_(8192)==9);
            BOOST_TEST(alloc.bucket_idx_from_size_(8193)==-1);


            BOOST_TEST(alloc.size_from_bucket_idx_(0)==16);
            BOOST_TEST(alloc.size_from_bucket_idx_(1)==32);
            BOOST_TEST(alloc.size_from_bucket_idx_(9)==8192);

        }


        static void test2(simple_caching_allocator & alloc) {
            auto ptr1  = boost::shared_ptr<void>{alloc.allocate(0), [&alloc](void* pointer){ alloc.deallocate(pointer, 0); }};
            BOOST_TEST(alloc.m_buckets.at(0).size()==0);

            ptr1.reset();
            BOOST_TEST(alloc.m_buckets.at(0).size()==1);

            auto ptr2  = boost::shared_ptr<void>{alloc.allocate(1), [&alloc](void* pointer){ alloc.deallocate(pointer, 1); }};
            BOOST_TEST(alloc.m_buckets.at(0).size()==0);

            ptr2.reset();
            BOOST_TEST(alloc.m_buckets.at(0).size()==1);



            BOOST_TEST(alloc.m_buckets.at(9).size()==0);
            auto ptr3  = boost::shared_ptr<void>{alloc.allocate(8192), [&alloc](void* pointer){ alloc.deallocate(pointer, 8192); }};
            BOOST_TEST(alloc.m_buckets.at(9).size()==0);

            ptr3.reset();
            BOOST_TEST(alloc.m_buckets.at(9).size()==1);


        }

        static void test3(simple_caching_allocator & alloc) {
            for(auto && bucket: alloc.m_buckets){  BOOST_TEST( bucket.size() == 0 ); }

            auto ptr1  = boost::shared_ptr<void>{alloc.allocate(8193), [&alloc](void* pointer){ alloc.deallocate(pointer, 8193); }};

            for(auto && bucket: alloc.m_buckets){  BOOST_TEST( bucket.size() == 0 ); }

            ptr1.reset();
            for(auto && bucket: alloc.m_buckets){  BOOST_TEST( bucket.size() == 0 ); }
        }


        struct test_t{
            int v1 =2;
            int v2 = 1;

        };


        static void test4() {
            gie::simple_caching_allocator alloc{4,13};

            {
                BOOST_TEST(alloc.m_alive_objects == 0);

                typedef gie::simple_to_std_allocator_t<test_t, decltype(alloc)> allocator_t;

                std::vector<test_t, allocator_t> vec{allocator_t{alloc}};

                vec.resize(33, test_t{33, 77});

                BOOST_TEST(alloc.m_alive_objects != 0);
            }

            {
                BOOST_TEST(alloc.m_alive_objects == 0);

                typedef gie::simple_to_std_allocator_t<test_t, decltype(alloc)> allocator_t;

                std::vector<test_t, allocator_t> vec{allocator_t{alloc}};

                vec.resize(33, test_t{33, 77});

                BOOST_TEST(alloc.m_alive_objects != 0);
            }
        }

        static void test5() {
             gie::simple_mt_allocator_t<gie::simple_caching_allocator> alloc{4,13};

            {
                BOOST_TEST(alloc.m_allocator_.m_alive_objects == 0);

                typedef gie::simple_to_std_allocator_t<test_t, decltype(alloc)> allocator_t;

                std::vector<test_t, allocator_t> vec{allocator_t{alloc}};

                vec.resize(33, test_t{33, 77});

                BOOST_TEST(alloc.m_allocator_.m_alive_objects != 0);
            }

            {
                BOOST_TEST(alloc.m_allocator_.m_alive_objects == 0);

                typedef gie::simple_to_std_allocator_t<test_t, decltype(alloc)> allocator_t;

                std::vector<test_t, allocator_t> vec{allocator_t{alloc}};

                vec.resize(33, test_t{33, 77});

                BOOST_TEST(alloc.m_allocator_.m_alive_objects != 0);
            }


        }


    };

}
//================================================================================================================================================

BOOST_AUTO_TEST_SUITE(simple_caching_allocator_test )
//================================================================================================================================================
BOOST_AUTO_TEST_CASE(test1_2) {
    gie::simple_caching_allocator alloc{4,13};
    gie::boost_test__simple_caching_allocator::test1(alloc);
    gie::boost_test__simple_caching_allocator::test2(alloc);
}

BOOST_AUTO_TEST_CASE(test3) {
    gie::simple_caching_allocator alloc{4,13};
    gie::boost_test__simple_caching_allocator::test3(alloc);
}


BOOST_AUTO_TEST_CASE(test4) {
    gie::boost_test__simple_caching_allocator::test4();
}

BOOST_AUTO_TEST_CASE(test5) {
    gie::boost_test__simple_caching_allocator::test5();
}

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
