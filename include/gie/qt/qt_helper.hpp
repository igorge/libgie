//================================================================================================================================================
// FILE: qt_helper.h
// (c) GIE 2011-08-19  21:21
//
//================================================================================================================================================
#ifndef H_GUARD_QT_HELPER_2011_08_19_21_21
#define H_GUARD_QT_HELPER_2011_08_19_21_21
//================================================================================================================================================

#include "stcrypt-qt-async-function.hpp"
#include "../debug.hpp"

#include <QObject>
#include <QCoreApplication>

#include <boost/thread/thread.hpp>

#include <functional>
#include <assert.h>
//================================================================================================================================================
struct gui_thread_dispatcher : QObject
{
    Q_OBJECT

    STCRYPT_DEF_EVENT_HANDLE(QObject);

public:
    template <class Func>
    void post( Func&& func){
    	GIE_DEBUG_LOG("enqueue to qt main thread dispatch from thread id "<<boost::this_thread::get_id());
		QCoreApplication::postEvent(this, new function_event_t( std::forward<Func>(func) ) );
    }

};

namespace gie { namespace qt {

	namespace impl201108192121 {

		template <class Dummy=int> struct holder_t{

			static gui_thread_dispatcher val;
		};

		template <class Dummy>
		gui_thread_dispatcher holder_t<Dummy>::val;
	}

	inline
	gui_thread_dispatcher& main_thread_dispatcher(){ return impl201108192121::holder_t<>::val; }

	template <class Func>
	void dispatch_on_main(Func&& func){ return main_thread_dispatcher().post( std::forward<Func>(func)); }

} }

//================================================================================================================================================
#endif
//================================================================================================================================================
