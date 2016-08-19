//================================================================================================================================================
// FILE: magic_cookie.h
// (c) GIE 2011-09-11  14:51
//
//================================================================================================================================================
#ifndef H_GUARD_MAGIC_COOKIE_2011_09_11_14_51
#define H_GUARD_MAGIC_COOKIE_2011_09_11_14_51
//================================================================================================================================================
#pragma once
//================================================================================================================================================]
#include <boost/mpl/int.hpp>
//================================================================================================================================================]
namespace gie {

	template <class Tag=boost::mpl::int_<0>, class CookieType = boost::uint32_t, CookieType cookie=0xcafebabe>
	struct cookie_checker {
		cookie_checker()
			#ifndef NDEBUG
			: m_debug_cookie( cookie )
			#endif
		{
			this->is_cookie_valid();
		}
		~cookie_checker(){
			#ifndef NDEBUG
			this->is_cookie_valid();
			m_debug_cookie=(CookieType)-1;
			#endif
		}

		void is_cookie_valid(){
			assert( m_debug_cookie == cookie );
		}

	private:
		#ifndef NDEBUG
		CookieType m_debug_cookie;
		#endif
	};

}
//================================================================================================================================================
#endif
//================================================================================================================================================
