//================================================================================================================================================
// FILE: self_impl.h
// (c) GIE 2017-08-18  22:53
//
//================================================================================================================================================
#ifndef H_GUARD_SELF_IMPL_2017_08_18_22_53
#define H_GUARD_SELF_IMPL_2017_08_18_22_53
//================================================================================================================================================
#pragma once
//================================================================================================================================================
namespace gie {

    template <class MixinType, class SelfT>
    struct self_impl {
        using self_type = SelfT;

    protected:
        self_type& self() {
            #ifndef NDEBUG
                auto const self = static_cast<SelfT *>( static_cast<MixinType*>(this) );
                self->assert_cookie_is_valid();
                return *self;
            #else
                return *static_cast<SelfT *>( static_cast<MixinType*>(this) );
            #endif
        }

        self_type const& self() const {
            #ifndef NDEBUG
                auto const self = static_cast<SelfT const*>(static_cast<MixinType const*>(this));
                self->assert_cookie_is_valid();
                return *self;
            #else
                return *static_cast<SelfT const *>( static_cast<MixinType const*>(this) );
            #endif
        }

    };


}
//================================================================================================================================================
#endif
//================================================================================================================================================
