//================================================================================================================================================
// FILE: gen_struct.h
// (c) GIE 2016-11-04  03:14
//
//================================================================================================================================================
#ifndef H_GUARD_GEN_STRUCT_2016_11_04_03_14
#define H_GUARD_GEN_STRUCT_2016_11_04_03_14
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <boost/preprocessor.hpp>
//================================================================================================================================================
#define GIE_SIO2_GEN( type_tag, fields_seq )  \
    /*type definition*/ \
    struct type_tag {   \
        BOOST_PP_SEQ_FOR_EACH_I( GIE_SIO2_GEN_ITEM_DEF, 0/*aux data*/, fields_seq ) \
    };                                                                              \
    /*serialization code*/                                                          \
    template <class Stream>                                                         \
    void serialize(type_tag & v, Stream& stream){                                   \
        BOOST_PP_SEQ_FOR_EACH_I( GIE_SIO2_GEN_SER_DEF, 0/*aux data*/, fields_seq )  \
    } \
/**/

#define GIE_SIO2_GEN_ITEM_DEF(r, aux/*aux data*/, i/*iter counter*/, item_def /*data*/) \
            ::boost::mpl::identity< BOOST_PP_TUPLE_ELEM(3/*size*/, 0/*extract idx*/, item_def /*tuple*/) >::type  \
            BOOST_PP_TUPLE_ELEM(3/*size*/, 1/*extract idx*/, item_def /*tuple*/);       \
            /**/


#define GIE_SIO2_GEN_SER_DEF(r, aux/*aux data*/, i/*iter counter*/, item_def /*data*/)      \
        stream(                                                                             \
            BOOST_PP_TUPLE_ELEM(3/*size*/, 2/*extract idx*/, item_def /*tuple*/) (          \
                v. BOOST_PP_TUPLE_ELEM(3/*size*/, 1/*extract idx*/, item_def /*tuple*/)     \
            )   \
        );      \
/**/

//================================================================================================================================================
namespace gie { namespace sio2 {
}}
//================================================================================================================================================
#endif
//================================================================================================================================================
