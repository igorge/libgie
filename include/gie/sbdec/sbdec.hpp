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
#include "gie/sio2/sio2_exceptions.hpp"

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/push_back.hpp>
#include <boost/fusion/include/filter_view.hpp>
#include <boost/fusion/include/flatten_view.hpp>
#include <boost/fusion/include/joint_view.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/fusion/include/filter.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/copy.hpp>
#include <boost/fusion/include/void.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/int.hpp>
//================================================================================================================================================
namespace gie { namespace sbdec {

        namespace sio2 = ::gie::sio2;

        using boost::fusion::void_;



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
            struct root : virtual ::gie::exception::root {};


            struct match_error : virtual root {};

        }

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

            template <class T>
            auto as(){
                return parser([parser= std::move(*this)](auto& reader, context_scope const *const scope)->T{
                    auto v = parser(reader, scope);
                    return boost::fusion::invoke( [](auto&& ... args){ return T{ std::move(args)...}; },  v);
                });
            }
        };

        template <class Parser>
        auto parser(Parser&& parser){
            return parser_t<Parser>(std::forward<Parser>(parser));
        }

        template <class Parser>
        auto ref(Parser const& parser){
            return parser_t<Parser const&>(parser);
        }

        #define GIE_FILTER_SIO2_EXCEPTIONS_EX(fun, scope) impl::filter_sio2_exceptions(fun, scope, __FILE__, __LINE__)
        #define GIE_FILTER_SIO2_EXCEPTIONS(fun) GIE_FILTER_SIO2_EXCEPTIONS_EX(fun, scope)

        namespace impl {


            template <class Fun>
            auto filter_sio2_exceptions(Fun&& fun, context_scope const * const scope, char const*const file, int const line){

                try{
                    return fun();
                } catch (sio2::exception::underflow const&){
                    std::string name{"<<UNEXPECTED EOF>>"};
                    context_scope tmp = {scope, &name};
                    GIE_THROW_EX( exception::match_error() << gie::exception::error_str_einfo( make_scope(&tmp) ), "<<unspecified>>", file, line );
                }

            }

            template <class Seq>
            using seq_size = typename boost::fusion::result_of::size< typename std::remove_reference<Seq>::type >::type;

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t<seq_size<Seq>::value==0> * dummy = nullptr)
            {
                return void_{} ;
            };

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t< (seq_size<Seq>::value>1) > * dummy = nullptr)
            {
//                using SeqFlat = boost::fusion::flatten_view<Seq>;
//                SeqFlat seq_flat(seq);

//                using vec_t = typename  boost::fusion::result_of::as_vector<SeqFlat>::type;
                using vec_t = typename  boost::fusion::result_of::as_vector< std::remove_reference_t<Seq> >::type;

                return boost::fusion::invoke( [](auto&& ... args){ return vec_t{ std::move(args)...}; },  seq);
            };

            template <class Seq>
            auto transform_seq(Seq&& seq, std::enable_if_t< seq_size<Seq>::value==1 > * dummy = nullptr)
            {
                return std::move(boost::fusion::front(std::forward<Seq>(seq) ));
            };


        }


        namespace impl{

            template <class T>
            auto& wrap_single(T& seq, std::enable_if_t< boost::fusion::traits::is_sequence <T>::value> * dummy = nullptr){
                return seq;
            }

            template <class T>
            auto wrap_single(T& value, std::enable_if_t< ! boost::fusion::traits::is_sequence <T>::value> * dummy = nullptr){
                static_assert( ! std::is_reference<T>::value );
                static_assert( ! std::is_const<T>::value );
                return boost::fusion::vector<T>{ std::move(value) };
            }

            template <class P1, class P2>
            auto make_sequence(P1&&p1, P2&&p2){
                return parser([p1 = std::forward<P1>(p1), p2 = std::forward<P2>(p2)] (auto& reader, context_scope const *const scope){

                    auto v1 = p1(reader, scope);
                    auto v2 = p2(reader, scope);

                    //auto r =  boost::fusion::vector<decltype(v1), decltype(v2)>(std::move(v1), std::move(v2));

                    decltype(auto)  seq1 = wrap_single(v1);
                    decltype(auto)  seq2 = wrap_single(v2);

                    auto joint_view  = boost::fusion::join(seq1, seq2);
                    auto filtered_view = boost::fusion::filter<boost::mpl::not_< boost::is_convertible<boost::mpl::_1, void_>>> (joint_view);


                    //boost::fusion::filter_view<decltype(r), boost::mpl::not_< boost::is_convertible<boost::mpl::_1, void_> > > view1{ r };

                    return impl::transform_seq( joint_view );
                });
            };

        }


        template <class P1, class P2>
        auto operator >> (parser_t<P1> && p1, P2&&p2){
            return impl::make_sequence(std::move(p1), std::move(p2));
        };

        template <class P1, class P2>
        auto operator >> (parser_t<P1> const& p1, P2 const& p2){
            return impl::make_sequence(p1, p2);
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

                GIE_FILTER_SIO2_EXCEPTIONS([&](){
                    reader(sio2::as<TypeTag>(tmp));
                });

                GIE_CHECK_EX( value == tmp, exception::match_error() << gie::exception::error_str_einfo( make_scope(scope) ) );

                return void_{};
            };
        }


        template <class TypeTag>
        auto value(){
            return parser([](auto& reader, context_scope const*const scope){

                return GIE_FILTER_SIO2_EXCEPTIONS([&]() {
                    typename TypeTag::base_type tmp;
                    reader(sio2::as<TypeTag>(tmp));

                    return tmp;
                });
            });
        }

        template <std::size_t counter, class InnerParser>
        auto repeat_n(InnerParser&& inner){

            return parser([inner=std::move(inner)](auto& reader, context_scope const*const scope){

                using counter_t = std::size_t;
                using elem_t = decltype( inner(reader, scope) );

                std::vector<elem_t> data;
                data.reserve(counter);

                for(counter_t i = 0; i<counter; ++i){
                    data.emplace_back( inner( reader, scope ) );
                }

                return data;
            });
        }


        template <class CounterTypeTag, class InnerParser>
        auto repeat(InnerParser&& inner){

            return parser([inner=std::move(inner)](auto& reader, context_scope const*const scope){

                using counter_t = typename CounterTypeTag::base_type;
                using elem_t = decltype( inner(reader, scope) );

                counter_t counter;
                GIE_FILTER_SIO2_EXCEPTIONS([&]() {
                    reader(sio2::as<CounterTypeTag>(counter));
                });

                std::vector<elem_t> data;
                data.reserve(counter);

                for(counter_t i = 0; i<counter; ++i){
                    data.emplace_back( inner( reader, scope ) );
                }

                return data;
            });
        }


} }
//================================================================================================================================================
#endif
//================================================================================================================================================
