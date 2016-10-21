//================================================================================================================================================
// FILE: caching_istream_iterator.h
// (c) GIE 2016-10-21  05:04
//
//================================================================================================================================================
#ifndef H_GUARD_CACHING_ISTREAM_ITERATOR_2016_10_21_05_04
#define H_GUARD_CACHING_ISTREAM_ITERATOR_2016_10_21_05_04
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/exceptions.hpp"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
//================================================================================================================================================
namespace gie {

    namespace impl {

        struct caching_istream_iterator_shared_t {

            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t const&) = delete;
            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t &&) = delete;
            caching_istream_iterator_shared_t& operator=(caching_istream_iterator_shared_t const&) = delete;

            using shared_page_t = boost::shared_ptr<std::vector<char>>;


            caching_istream_iterator_shared_t(std::istream& p_is, size_t const p_page_size, unsigned int const p_page_count)
                    : m_is(p_is)
                    , m_page_size(p_page_size)
            {
            }


        private:
            shared_page_t alloc_page(){
                return boost::make_shared<shared_page_t::element_type>();
            }

            std::istream& m_is;
            size_t const m_page_size;
        };

    }


//    struct caching_istream_iterator_t
//            : boost::iterator_facade<caching_istream_iterator_t, char, boost::forward_traversal_tag>
//    {
//        friend class boost::iterator_core_access;
//
//        void increment(){
//            GIE_UNEXPECTED();
//        }
//
//        dereference() const
//
//    };
}
//================================================================================================================================================
#endif
//================================================================================================================================================
