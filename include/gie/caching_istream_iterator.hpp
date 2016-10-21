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
#include "gie/simple_lru.hpp"
#include "gie/exceptions.hpp"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
//================================================================================================================================================
namespace gie {

    namespace impl {

        struct caching_istream_iterator_shared_t {

            using page_index_t = unsigned int;
            using page_count_t = unsigned int;
            using streampos_t = std::streamoff; // c++11: It (std::streamoff) is a typdef of one the fundamental signed integral types large enough to represent the maximum possible file size supported by the system.
            using streamsize_t = streampos_t;

            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t const&) = delete;
            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t &&) = delete;
            caching_istream_iterator_shared_t& operator=(caching_istream_iterator_shared_t const&) = delete;

            using shared_page_type = boost::shared_ptr<std::vector<char>>;
            using lru_type = lru_t<page_index_t, shared_page_type, std::allocator>;


            caching_istream_iterator_shared_t(std::istream& p_is, size_t const p_page_size, page_index_t const p_cache_page_count)
                    : m_is(p_is)
                    , m_page_size(p_page_size)
                    , m_stream_size(stream_size(p_is))
                    , m_stream_page_count(calc_max_page( m_stream_size, p_page_size))
                    , m_lru(p_cache_page_count, std::allocator<void>{})
            {
                assert(m_page_size>1);
                assert(m_page_size<=std::numeric_limits<streampos_t>::max());
                assert(m_stream_page_count>0);
                assert(m_is.good());

                m_is.exceptions(std::ios::goodbit);
            }

            shared_page_type const& get_page(page_index_t const idx){

                if(idx>=m_stream_page_count) return m_empty_page_ptr;

                auto const& r = m_lru.find(idx);
                if(r) {
                    GIE_DEBUG_LOG("GET PAGE HIT : "<< idx);
                    return *r;
                } else {
                    GIE_DEBUG_LOG("GET PAGE MISS : "<< idx);
                    return read_page_(idx);
                }

            }

            size_t size()const{
                return m_stream_size;
            }


        private:

            shared_page_type const& read_page_(page_index_t const idx){

                assert(!m_lru.find(idx));

                auto new_page = do_read_page_(idx);
                if(new_page){
                    return m_lru.insert(idx, std::move(new_page) );
                } else {
                    return m_empty_page_ptr;
                }

            }

            shared_page_type do_read_page_(page_index_t const idx){

                auto new_page = alloc_page();

                streampos_t const current_pos =  m_is.tellg();
                GIE_CHECK(!m_is.bad());

                auto const abs_pos = pos_from_page_idx_(idx);
                if(current_pos != abs_pos){
                    if(m_is.eof()) m_is.clear();
                    m_is.seekg(abs_pos);
                    GIE_CHECK(m_is.good());
                }

                m_is.read(new_page->data(), new_page->size());

                GIE_CHECK(!m_is.bad()); //maybe eof

                auto const read_bytes = m_is.gcount();
                GIE_CHECK(read_bytes > 0 && read_bytes<=static_cast<streampos_t>(m_page_size));

                if(read_bytes!=new_page->size()){ // eof detected
                    assert(m_is.eof());
                    new_page->resize(read_bytes);
                }

                return new_page;
            }

            streampos_t pos_from_page_idx_(page_index_t const idx)const{
                return idx*m_page_size;
            }

            shared_page_type alloc_page(){
                auto const& page =  boost::make_shared<shared_page_type::element_type>();
                page->resize(m_page_size);
                return page;
            }

            static streamsize_t stream_size(std::istream& is){
                is.seekg(0, std::ios_base::end);
                auto const s = is.tellg();
                GIE_CHECK(is.good());

                return s;
            }

            static page_index_t calc_max_page(streamsize_t const p_ssize, size_t page_size ){
                assert(p_ssize>1);
                assert(p_ssize>1);

                size_t const ssize = static_cast<size_t>(p_ssize);

                assert(ssize>=page_size);

                auto const m = ssize / page_size;
                auto const r = ssize % page_size;

                return m + (r==0?0:1);
            }

            std::istream& m_is;
            size_t const m_page_size;
            streampos_t const m_stream_size;
            page_index_t const m_stream_page_count;
            lru_type m_lru;

            shared_page_type const m_empty_page_ptr{};
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
