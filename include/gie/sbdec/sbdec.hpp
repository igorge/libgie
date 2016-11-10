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

#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/flatten.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/eval_if.hpp>
#include <boost/hana/unpack.hpp>


#include <boost/type_traits/is_convertible.hpp>
//================================================================================================================================================
namespace gie { namespace sbdec {

        using namespace boost::hana::literals;

        namespace sio2 = ::gie::sio2;
        namespace hana = ::boost::hana;

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


        }

        struct parser_tag {};


        template <class Parser>
        struct parser_t : parser_tag {
            parser_t(Parser&& parser) : m_parser (std::forward<Parser>(parser))
            {
            }

            template <class ReaderT>
            auto operator()(ReaderT& reader, context_scope const *const scope)const{

                return hana::eval_if( hana::is_a<hana::tuple_tag, decltype(m_parser( reader, scope ))>,
                               [&](auto _){return m_parser( reader, scope );},
                               [&](auto _){return hana::make_tuple(m_parser( reader, scope ) );}
                );
            }

            Parser m_parser;

            auto name(std::string&& name){
                return with_name( std::move(name), *this );
            }

            template <template <class ...> class T>
            auto as(){
                return parser([parser= std::move(*this)](auto& reader, context_scope const *const scope){

                    static_assert( hana::is_a<hana::tuple_tag, decltype(parser(reader, scope))> );

                    return hana::unpack( parser(reader, scope),
                                         [](auto&& ...args){

                                             using as_t = T< std::remove_reference_t<decltype(args)> ... >;

                                             return as_t{ std::forward<decltype(args)>(args) ... };
                                         }
                    );


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


        namespace impl{

            template <class P1, class P2>
            auto make_sequence(P1&&p1, P2&&p2){
                return parser([p1 = std::forward<P1>(p1), p2 = std::forward<P2>(p2)] (auto& reader, context_scope const *const scope){


                    static_assert( ! std::is_reference<decltype(p1(reader, scope))>::value );
                    static_assert( ! std::is_reference<decltype(p2(reader, scope))>::value );

                    auto v1 = p1(reader, scope);
                    auto v2 = p2(reader, scope);

                    return hana::concat( std::move(v1), std::move(v2) );
                });
            };


            template <class T>
            decltype(auto) unlift(T&& v, std::enable_if_t< ! hana::is_a<hana::tuple_tag, T>() > * const dummy = nullptr){
                return std::forward<T>(v);
            }

            template <class T>
            decltype(auto) unlift(T&&v, std::enable_if_t< (hana::is_a<hana::tuple_tag, T>() && decltype(hana::size(v))::value == 1) > * const dummy = nullptr){
                static_assert( std::is_reference<decltype(v[0_c])>::value );
                static_assert( std::is_lvalue_reference<decltype(v[0_c])>::value );
                return v[0_c];
            }

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

                return hana::make_tuple();
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
                using elem_t = std::remove_reference_t<decltype( impl::unlift( inner( reader, scope ) ) )>;

                std::vector<elem_t> data;
                data.reserve(counter);

                for(counter_t i = 0; i<counter; ++i){
                    data.emplace_back( std::move(impl::unlift( inner( reader, scope ) ) ) );
                }

                return data;
            });
        }


        template <class CounterTypeTag, class InnerParser>
        auto repeat(InnerParser&& inner){

            return parser([inner=std::move(inner)](auto& reader, context_scope const*const scope){

                using counter_t = typename CounterTypeTag::base_type;
                using elem_t = std::remove_reference_t<decltype( impl::unlift( inner( reader, scope ) ) )>;

                counter_t counter;
                GIE_FILTER_SIO2_EXCEPTIONS([&]() {
                    reader(sio2::as<CounterTypeTag>(counter));
                });

                std::vector<elem_t> data;
                data.reserve(counter);

                for(counter_t i = 0; i<counter; ++i){
                    data.emplace_back( std::move(impl::unlift( inner( reader, scope ) ) ) );
                }

                return data;
            });
        }


} }
//================================================================================================================================================
#endif
//================================================================================================================================================
