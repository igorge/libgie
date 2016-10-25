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
#include "gie/simple_to_std_allocator.hpp"
#include "gie/type_util.hpp"
#include "gie/allocator/construct_new.hpp"
#include "gie/simple_lru.hpp"
#include "gie/exceptions.hpp"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range.hpp>
#include <boost/integer.hpp>

#include <utility>
#include <limits>
//================================================================================================================================================
namespace gie {

    namespace impl {

        template <class AllocatorT>
        struct caching_istream_iterator_shared_t {

            using page_index_t = unsigned int;
            using page_count_t = unsigned int;
            using streampos_t = std::streamoff; // c++11: It (std::streamoff) is a typdef of one the fundamental signed integral types large enough to represent the maximum possible file size supported by the system.
            using streamsize_t = streampos_t;

            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t const&) = delete;
            caching_istream_iterator_shared_t(caching_istream_iterator_shared_t &&) = delete;
            caching_istream_iterator_shared_t& operator=(caching_istream_iterator_shared_t const&) = delete;

            using allocator_t = AllocatorT;

            using page_type =  std::vector<char, typename std::allocator_traits<AllocatorT>::template rebind_alloc<char> >;
            using shared_page_type = boost::shared_ptr< page_type >;
            using weak_shared_page_type = boost::weak_ptr< page_type >;
            using lru_type = lru_t<page_index_t, shared_page_type, allocator_t>;


            template <class AnyCompatibleAllocator>
            caching_istream_iterator_shared_t(AnyCompatibleAllocator && p_allocator, std::istream& p_is, size_t const p_page_size, page_count_t const p_cache_page_count)
                    : m_allocator(std::forward<AnyCompatibleAllocator>(p_allocator))
                    , m_is(p_is)
                    , m_page_size(p_page_size)
                    , m_stream_size(stream_size(p_is))
                    , m_stream_page_count(calc_max_page( m_stream_size, p_page_size))
                    , m_lru(p_cache_page_count, m_allocator)
            {
                assert(m_page_size>1);
                assert(m_page_size<=std::numeric_limits<streampos_t>::max());
                assert(m_stream_page_count>=0);
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

            streamsize_t stream_size()const{
                assert(m_stream_size>=0);
                return m_stream_size;
            }

            size_t page_size()const{
                return m_page_size;
            }

            std::pair<page_index_t, size_t> get_page_index_from_position(streampos_t const p_pos){
                typedef gie::uint_can_hold_digits<decltype(p_pos), size_t>::type u_type;

                assert(p_pos>=0);
                assert(p_pos<m_stream_size);

                auto const pos = static_cast<u_type>(p_pos);

                auto const m = pos / m_page_size;
                auto const r = pos % m_page_size;

                return {m,r};
            };

            auto& get_allocator(){
                return m_allocator;
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

                if(read_bytes!=static_cast<streampos_t>(new_page->size())){ // eof detected
                    assert(m_is.eof());
                    new_page->resize(read_bytes);
                }

                return new_page;
            }

            streampos_t pos_from_page_idx_(page_index_t const idx)const{
                return idx*m_page_size;
            }

            shared_page_type alloc_page(){
                auto const page_raw_ptr = construct_new<page_type>(m_allocator, m_allocator);
                shared_page_type page{page_raw_ptr, [allocator=m_allocator](page_type* pointer)mutable{ destroy_free(allocator, pointer); }, m_allocator};
                page->resize(m_page_size);
                return page;
            }

            static streamsize_t stream_size(std::istream& is){
                GIE_CHECK(!is.bad());

//                { // workaround for empty boost::iostreams::stream, which fails at seekg(0, std::ios_base::end)
//                    is.seekg(0);
//                    if(!is.good()){
//                        GIE_DEBUG_LOG("Empty sized stream or seekg is not supported.");
//                        is.clear();
//                        return 0;
//                    }
//                }

                is.seekg(0, std::ios_base::end);
                auto const s = is.tellg();
                GIE_CHECK(is.good());

                return s;
            }

            static page_index_t calc_max_page(streamsize_t const p_ssize, size_t page_size ){
                assert(p_ssize>=0);
                assert(page_size>1);

                typedef gie::uint_can_hold_digits<decltype(p_ssize), decltype(page_size)>::type u_type;

                u_type const ssize = p_ssize;

                auto const m = ssize / page_size;
                auto const r = ssize % page_size;

                return m + (r==0?0:1);
            }

            allocator_t m_allocator;
            std::istream& m_is;
            size_t const m_page_size;
            streamsize_t const m_stream_size;
            page_index_t const m_stream_page_count;
            lru_type m_lru;

            shared_page_type const m_empty_page_ptr{};
        };

    }


    template <class AllocatorT>
    struct caching_istream_iterator_t
            : boost::iterator_facade<caching_istream_iterator_t<AllocatorT>, char, boost::forward_traversal_tag, char const&>
    {
        friend class boost::iterator_core_access;

        using self_type = caching_istream_iterator_t<AllocatorT>;

        typedef impl::caching_istream_iterator_shared_t<AllocatorT> impl_type;
        typedef boost::shared_ptr<impl_type> shared_impl_type;

        typedef typename impl_type::streampos_t  position_t;
        typedef typename impl_type::page_count_t page_count_t;
        typedef typename impl_type::page_index_t page_index_t;
        typedef typename impl_type::shared_page_type shared_page_type;
        typedef typename impl_type::weak_shared_page_type weak_shared_page_type;

        caching_istream_iterator_t(){};

        template <class AnyCompatibleAllocator>
        caching_istream_iterator_t(AnyCompatibleAllocator && p_allocator, std::istream& is, size_t const p_page_size, page_count_t const p_cache_page_count)
                : m_impl(make_shared_(std::forward<AnyCompatibleAllocator>(p_allocator), is, p_page_size, p_cache_page_count) )
        {
        };


        bool equal(self_type const& other)const{

            if(this->m_impl == nullptr) { // normalize: l - iter, r - end marker
                if( other.m_impl== nullptr ) return true;
                return other.equal(*this);
            }

            assert(m_position>=0);
            assert(m_position<=m_impl->stream_size());

            if(other.m_impl==nullptr){
                if(m_impl->stream_size()==m_position) return true;

                return false;
            }

            return m_position==other.m_position;
        }

        void increment(){
            assert(m_impl);
            assert(m_position < m_impl->stream_size());
            assert(m_rel_position < m_impl->page_size());

            ++m_position;

            if(auto const page = m_current_page.lock()){
                ++m_rel_position;

                assert(m_rel_position <= page->size());

                if(m_rel_position==page->size()){
                    m_current_page.reset();
                    m_rel_position = 0;
                }
            }
        }


        typename self_type::reference dereference()const{
            assert(m_impl);
            assert(m_position < m_impl->stream_size());
            assert(m_rel_position < m_impl->page_size());

            auto const& page = lazy_load_page_();
            assert(m_rel_position < page->size());

            return (*page)[m_rel_position];
        }


    private:

//        auto& get_allocator()const{
//            assert(m_impl);
//            return m_impl->get_allocator();
//        }

        template <class AnyCompatibleAllocator>
        static shared_impl_type make_shared_(AnyCompatibleAllocator const& p_allocator, std::istream& p_is, size_t const p_page_size, page_count_t const p_cache_page_count){
            auto * const raw_impl = gie::construct_new<impl_type>(p_allocator, p_allocator, p_is, p_page_size, p_cache_page_count);
            shared_impl_type impl{raw_impl, [alloc = p_allocator](impl_type * pointer){gie::destroy_free(alloc, pointer);} };
            return impl;
        }

        shared_page_type lazy_load_page_()const{
            if(auto current_page = m_current_page.lock()){
                return current_page;
            } else {
                auto const idx = m_impl->get_page_index_from_position(m_position);
                auto page = m_impl->get_page(idx.first);
                m_current_page = page;
                m_rel_position = idx.second;
                return page;
            };
        }

    private:

        position_t m_position = 0;
        mutable size_t     m_rel_position = 0;
        mutable weak_shared_page_type m_current_page;

        shared_impl_type m_impl;
    };


    template < class AllocatorT>
     auto make_istream_range(AllocatorT && allocator, std::istream& is, size_t const page_size, typename caching_istream_iterator_t<AllocatorT>::page_count_t const pages_to_cache){
        return boost::make_iterator_range(
                caching_istream_iterator_t<AllocatorT>{ std::forward<AllocatorT>(allocator), is, page_size, pages_to_cache},
                caching_istream_iterator_t<AllocatorT>{} );
    }
}
//================================================================================================================================================
#endif
//================================================================================================================================================
