//================================================================================================================================================
// FILE: sbdec.h
// (c) GIE 2016-11-08  18:03
//
//================================================================================================================================================
#ifndef H_GUARD_SBDEC_2016_11_08_18_03
#define H_GUARD_SBDEC_2016_11_08_18_03
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/exceptions.hpp"
#include "gie/sio2/sio2_core.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/filter_view.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/copy.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/int.hpp>
//================================================================================================================================================
namespace gie { namespace sbdec {

        namespace sio2 = ::gie::sio2;



        struct context_scope {
            context_scope const* previous;
            std::string const*   name;
        };

        std::string make_scope(context_scope const * const scope){
            auto * current = scope;

            std::string tmp;

            while(current) {
                if(current->name!= nullptr) {
                    tmp += "@";
                    tmp += *(current->name);
                }

                current = current->previous;
            }

            return tmp;
        }


        namespace exception {
            struct root : virtual ::gie::exception::root, virtual std::exception {
                explicit root(context_scope const * const context)
                    : m_context( make_scope(context) )
                {}

                char const * what() const noexcept override {
                    return m_context.c_str();
                }

            private:
                std::string m_context;
            };

        }

        struct ignore_t {};

        struct parser_tag {};


        template <class Parser>
        struct parser_t : parser_tag {
            parser_t(Parser&& parser) : m_parser (std::forward<Parser>(parser))
            {
            }

            template <class ReaderT>
            auto operator()(ReaderT& reader, context_scope const *const scope)const{
                return m_parser( reader, scope );
            }

            Parser m_parser;

            auto name(std::string&& name){
                return with_name( std::move(name), *this );
            }
        };

        template <class Parser>
        auto parser(Parser&& parser){
            return parser_t<Parser>(std::forward<Parser>(parser));
        }


        namespace impl {

            template <class Seq>
            using seq_size = typename boost::fusion::result_of::size< typename std::remove_reference<Seq>::type >::type;

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t<seq_size<Seq>::value==0> * dummy = nullptr)
            {
                return ignore_t{} ;
            };

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t< (seq_size<Seq>::value>1) > * dummy = nullptr)
            {
                using vec_t = typename  boost::fusion::result_of::as_vector<Seq>::type;

                return boost::fusion::invoke( [](auto&& ... args){ return vec_t{ std::move(args)...}; },  seq);
            };

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t< seq_size<Seq>::value==1 > * dummy = nullptr)
            {
                return std::move(boost::fusion::front(std::forward<Seq>(seq) ));
            };


        }

        template <class P1, class P2>
        auto operator >> (parser_t<P1> && p1, P2&&p2){
            return parser([p1 = std::move(p1), p2 = std::move(p2)] (auto& reader, context_scope const *const scope){

                auto v1 = p1(reader, scope);
                auto v2 = p2(reader, scope);

                auto r =  boost::fusion::vector<decltype(v1), decltype(v2)>(std::move(v1), std::move(v2));

                boost::fusion::filter_view<decltype(r), boost::mpl::not_< boost::is_convertible<boost::mpl::_1, ignore_t> > > view1{ r };

                return impl::transform_seq( std::move(view1) );
            });
        };


        template <class Parser>
        auto with_name(std::string && scope_name, Parser&& p){
            return parser([name=std::move(scope_name), p=std::forward<decltype(p)>(p)](auto& reader, context_scope const *const scope){
                context_scope current_scope = {scope, &name};
                return p(reader, &current_scope);
            });
        }

        template <class TypeTag>
        auto constant(typename TypeTag::base_type const value){
            return [value](auto& reader, context_scope const*const scope){

                typename TypeTag::base_type tmp;
                reader( sio2::as<TypeTag>(tmp) );
                GIE_CHECK_EX( value == tmp, exception::root(scope) );

                return ignore_t{};
            };
        }


        template <class TypeTag>
        auto value(){
            return [](auto& reader, context_scope const*const scope){

                typename TypeTag::base_type tmp;
                reader( sio2::as<TypeTag>(tmp) );

                return tmp;
            };
        }



} }
//================================================================================================================================================
#endif
//================================================================================================================================================
