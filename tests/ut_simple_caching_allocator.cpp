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


    using simple_alloc_t = gie::simple_caching_allocator;
    template <class T> using alloc_aligned_tt = gie::simple_to_std_aligned_allocator_t<T, simple_alloc_t>;
    template <class T> using alloc_tt = gie::simple_to_std_non_aligned_allocator_t<T, simple_alloc_t>;


    BOOST_AUTO_TEST_CASE(xxx_to_std_xxx_allocator_test_01) {

        gie::simple_caching_allocator alloc{4, 13};
        gie::simple_caching_allocator alloc2{4, 13};


        alloc_aligned_tt<void> a1{alloc};
        alloc_aligned_tt<int> a2{a1};
        alloc_aligned_tt<char> a3{alloc};

        BOOST_CHECK(a1 == a1);
        BOOST_CHECK(a1 == a2);
        BOOST_CHECK(a1 == a3);

        BOOST_CHECK(a2 == a2);
        BOOST_CHECK(a2 == a3);

        BOOST_CHECK(a3 == a3);


        BOOST_CHECK(!(a1 != a1));
        BOOST_CHECK(!(a1 != a2));
        BOOST_CHECK(!(a1 != a3));

        BOOST_CHECK(!(a2 != a2));
        BOOST_CHECK(!(a2 != a3));

        BOOST_CHECK(!(a3 != a3));


        alloc_aligned_tt<void> a1_2{alloc2};
        alloc_aligned_tt<int> a2_2{a1_2};
        alloc_aligned_tt<char> a3_2{alloc2};

        BOOST_CHECK(a1_2 != a1);
        BOOST_CHECK(a2_2 != a2);
        BOOST_CHECK(a3_2 != a3);
    }


    BOOST_AUTO_TEST_CASE(xxx_to_std_xxx_allocator_test_02) {

        gie::simple_caching_allocator alloc{4, 13};
        gie::simple_caching_allocator alloc2{4, 13};


        alloc_tt<void> a1{alloc};
        alloc_tt<int> a2{a1};
        alloc_tt<char> a3{alloc};

        BOOST_CHECK(a1 == a1);
        BOOST_CHECK(a1 == a2);
        BOOST_CHECK(a1 == a3);

        BOOST_CHECK(a2 == a2);
        BOOST_CHECK(a2 == a3);

        BOOST_CHECK(a3 == a3);


        BOOST_CHECK(!(a1 != a1));
        BOOST_CHECK(!(a1 != a2));
        BOOST_CHECK(!(a1 != a3));

        BOOST_CHECK(!(a2 != a2));
        BOOST_CHECK(!(a2 != a3));

        BOOST_CHECK(!(a3 != a3));


        alloc_tt<void> a1_2{alloc2};
        alloc_tt<int> a2_2{a1_2};
        alloc_tt<char> a3_2{alloc2};

        BOOST_CHECK(a1_2 != a1);
        BOOST_CHECK(a2_2 != a2);
        BOOST_CHECK(a3_2 != a3);
    }

//================================================================================================================================================
BOOST_AUTO_TEST_SUITE_END()
//================================================================================================================================================
