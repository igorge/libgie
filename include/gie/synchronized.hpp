//================================================================================================================================================
// FILE: synchronized.h
// (c) GIE 2017-08-04  00:00
//
//================================================================================================================================================
#ifndef H_GUARD_SYNCHRONIZED_2017_08_04_00_00
#define H_GUARD_SYNCHRONIZED_2017_08_04_00_00
//================================================================================================================================================
#pragma once
//================================================================================================================================================
namespace gie {

    template <typename TagType = void>
    struct with_synchronized {

        template <typename Fun>
        decltype(auto) synchronized(Fun&& fun) const {
            std::lock_guard guard{m_this_mutex};
            return fun();
        }

        template <typename Fun>
        decltype(auto) synchronized(Fun&& fun) {
            std::lock_guard guard{m_this_mutex};
            return fun();
        }

    private:
        std::mutex m_this_mutex;


    };

}
//================================================================================================================================================
#endif
//================================================================================================================================================