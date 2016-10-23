//================================================================================================================================================
// FILE: simple_lru.h
// (c) GIE 2016-10-20  17:10
//
//================================================================================================================================================
#ifndef H_GUARD_SIMPLE_LRU_2016_10_20_17_10
#define H_GUARD_SIMPLE_LRU_2016_10_20_17_10
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/exceptions.hpp"
#include "gie/log/debug.hpp"

#include <boost/optional.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/optional.hpp>
//================================================================================================================================================
namespace gie {
    template <class KeyT, class T, class AllocatorT>
    struct lru_t {


        struct value_type {

            value_type(value_type const& v)
                    : key(v.key)
                    , value (v.value)
            {}

            value_type(value_type && v)
                    : key( std::move(v.key) )
                    , value( std::move(v.value) )
            {}

            template <class P1, class P2>
            value_type(P1&& p_key, P2&& p_value)
                    : key( std::forward<P1>(p_key) )
                    , value( std::forward<P2>(p_value) )
            {}


            KeyT key;
            T    value;
        };

        typedef typename std::allocator_traits<AllocatorT>::template rebind_alloc<value_type> allocator_t;


        typedef boost::multi_index_container<
                value_type,
                boost::multi_index::indexed_by<
                        boost::multi_index::ordered_unique<boost::multi_index::member<value_type, KeyT, &value_type::key> >,
                        boost::multi_index::sequenced<>
                >,
                allocator_t
        > index_t;

        template <class AllocatorCT>
        lru_t(unsigned int p_count, AllocatorCT&& allocator)
            : m_container( std::forward<AllocatorCT>(allocator) )
            , m_max_count(p_count)
        {
            assert(p_count>1);
        }

        lru_t(lru_t const&) = delete;
        lru_t& operator=(lru_t const&) = delete;


        template <class P1, class P2>
        T const& insert(P1&& key, P2&& value){

            auto& index0 = m_container.template get<0>();
            assert(index0.size()<=m_max_count);

            auto const& r = index0.find(key);

            if(r==index0.end()){
                GIE_DEBUG_LOG("index for key '"<<key<<"' not found, inserting...");
                if(index0.size()==m_max_count){
                    shrink_();
                }

                auto const& r2 =  index0.emplace( std::forward<P1>(key),  std::forward<P2>(value) );
                GIE_CHECK(r2.second);

                return r2.first->value;
            } else {
                GIE_DEBUG_LOG("index for key '"<<key<<"' found, replacing...");
                GIE_CHECK( index0.modify(r, [&value](value_type& v){v.value=std::forward<P2>(value);}) );

                update_used_(r);

                return r->value;
            }
        }

        boost::optional<T const &> find(KeyT const& key){

            auto const & index0 = m_container.template get<0>();

            auto const& r = index0.find(key);

            if(r==index0.end()){
                return {};
            } else {
                update_used_(r);
                return r->value;
            }


        }


        auto const& get_lru_list()const{
            return m_container.template get<1>();
        }



    private:
        template <class Iter>
        void update_used_(Iter& iter){
            GIE_DEBUG_LOG("update used for '"<< iter->key << "'.");

            auto& index1 = m_container.template get<1>();
            auto const& i1_iter = m_container.template project<1>(iter);
            assert(i1_iter!=index1.end());

            index1.relocate(index1.end(), i1_iter);
        }

        void shrink_(){
            assert(!m_container.empty());
            auto& index1 = m_container.template get<1>();

            GIE_DEBUG_LOG("evicting oldest entry with key '"<<index1.begin()->key<<"' ...");

            index1.erase(index1.begin());
            assert(m_container.size()==m_max_count-1);
        }


    private:
        index_t m_container;
        unsigned int const m_max_count;

    };
}
//================================================================================================================================================
#endif
//================================================================================================================================================
