//================================================================================================================================================
// FILE: zmq.h
// (c) GIE 2011-08-25  16:45
//
//================================================================================================================================================
#ifndef H_GUARD_ZMQ_2011_08_25_16_45
#define H_GUARD_ZMQ_2011_08_25_16_45
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/misc.hpp"
#include "gie/exceptions.hpp"
#include <zmq.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>
//================================================================================================================================================
namespace gie { namespace z {

	namespace exception {
		struct root : virtual gie::exception::root {};
		struct zmq_e : virtual root {};
		struct connection_refused_t : virtual zmq_e {};
		struct term_e : virtual root {};

		typedef boost::error_info< struct tag_zmq_errno_einfo, int > zmq_errno_einfo;
	}

	void e_filter_zmq_exceptions(){
		try{
			throw;
		} catch(zmq::error_t const&e){
			GIE_DEBUG_LOG( "zmq errno: "<<e.num() );
			GIE_THROW_IF(e.num()==ETERM, 		exception::term_e() << boost::errinfo_nested_exception( boost::copy_exception(e)) );
			GIE_THROW_IF(e.num()==ECONNREFUSED, exception::connection_refused_t() << boost::errinfo_nested_exception( boost::copy_exception(e)) );
			GIE_THROW( exception::zmq_e() << boost::errinfo_nested_exception( boost::copy_exception(e)) );
		}
	}



	namespace impl {
		struct holds_vector_with_parent_ref {};
	}

	template <class Vector>
	struct cache_t {
		typedef cache_t<Vector> this_type;

		struct vector_t : Vector, impl::holds_vector_with_parent_ref, boost::noncopyable {

			explicit
			vector_t(this_type* const parent) : m_parent( parent ) {
				GIE_DEBUG_TRACE_INOUT();
				#ifndef NDEBUG
				m_d_magic=0xbadf00d;
				#endif
			}

			void check_state_()const{
				#ifndef NDEBUG
				assert(m_d_magic==0xbadf00d);
				#endif
			}

			~vector_t(){
				GIE_DEBUG_TRACE_INOUT();
				this->check_state_();
			}

			this_type* manager()const{ this->check_state_(); return m_parent; }
		private:
			this_type* m_parent;
			#ifndef NDEBUG
			unsigned int m_d_magic;
			#endif
		};

		typedef std::unique_ptr<vector_t> vector_ptr_t;


		typedef typename Vector::value_type value_type;

		vector_ptr_t alloc(size_t const size_hint=0){
			GIE_TMPL_DECLTYPE(m_this_lock)::scoped_lock lock(m_this_lock);
			GIE_DEBUG_TRACE_INOUT();

			if(m_cache.empty()) {
				GIE_DEBUG_LOG("MISS");
				vector_ptr_t tmp( new vector_t(this) );
				return tmp;
			} else {
				GIE_DEBUG_LOG("HIT");
				vector_ptr_t tmp (  m_cache.back() );
				m_cache.resize( m_cache.size()-1 );
				return tmp;
			}
		}

		void reuse(vector_t* const vec){
			assert(vec);
			GIE_DEBUG_TRACE_INOUT();


                        try{
                            //GIE_TMPL_DECLTYPE(m_this_lock)::scoped_lock lock(m_this_lock); // mingw 4.5.2 seg fault here, wtf
                            boost::mutex::scoped_lock lock(m_this_lock);

				vec->clear();
				m_cache.push_back( vec );
			}catch(...){
				delete vec;
				throw;
			}

		}

		~cache_t(){
			GIE_DEBUG_TRACE_INOUT();
			boost::for_each(m_cache, [](vector_t*& vec){
				delete vec;
				vec=0;
			});
		}

	private:
		boost::mutex m_this_lock;
		std::vector< vector_t* > m_cache;
	};


	template <class E>
	E&&
	fill_zmq_exception_info_from_errno(E&&e, int const errnum){
		e << exception::zmq_errno_einfo(errnum) << gie::exception::error_str_einfo( zmq_strerror (errnum) );
		return std::forward<E>( e );
	}

	#define GIE_Z_THROW() GIE_Z_THROW_FROM_CODE(zmq_errno())
	#define GIE_Z_THROW_IF(cond) do { if(cond) GIE_Z_THROW_FROM_CODE(zmq_errno()); } while(false)
	#define GIE_Z_THROW_FROM_CODE(errnum) ::gie::z::throw_exception_from_err_code(errnum)

	void throw_exception_from_err_code(int const errnum){
		assert(errnum);

		#define GIE_Z_E_MAP(code, e) case code: GIE_THROW( fill_zmq_exception_info_from_errno(e, code) )

		switch(errnum){

		GIE_Z_E_MAP(ETERM, exception::term_e() );

		default: GIE_THROW( fill_zmq_exception_info_from_errno(exception::zmq_e(), errnum) );
		}

		#undef GIE_Z_E_MAP
	}

	struct socket_t;
	struct message_t;


	template <class Fun, class Checker>
	auto zmq_retry(Fun&&fun, Checker&&is_failed)
			->typename std::remove_reference<decltype(fun())>::type
	{
		typedef typename std::remove_reference<decltype(fun())>::type fun_ret_t;
		for(fun_ret_t rc;;){
			rc = fun();
			if(is_failed(rc)){
				auto const err = zmq_errno();
				if(err==EINTR){
					continue;
				} else {
					GIE_Z_THROW_FROM_CODE(err);
				}
			} else {
				return rc;
			}
		}//end for
	}

	struct context_t : boost::noncopyable {

		context_t(int const io_threads = 1)
			: m_is_termed(false)
		{
			m_context = zmq_init (io_threads);
			GIE_Z_THROW_IF( !m_context );
		}


        ~context_t(){
        	GIE_DEBUG_TRACE_INOUT();
        	try{
        		this->term();
        	}catch(...){
        		GIE_UNEXPECTED_IN_DTOR();
        	}
        }

        void term(){

        	{
				GIE_TMPL_DECLTYPE(m_self_lock)::scoped_lock lock(m_self_lock);
				GIE_DEBUG_IF_LOG(this->m_is_termed, "already termed");
				if(this->m_is_termed)
					return;
				else
					this->m_is_termed = true;
        	}

        	this->term_(); // can block here, but as for other threads we are termed
        }

        template<typename... Args> socket_t socket(Args&&... args); // thread safe

        void* get()const {return m_context;}
	private:

		void term_(){ // does not 'touch' this data, so t.s.
			GIE_BT_BEGIN

			GIE_DEBUG_TRACE_INOUT();
			GIE_DEBUG_LOG("zmq_term: "<< m_context);

			zmq_retry(
				[&]{return zmq_term (m_context);},
				[&](int rc){return rc;});

			GIE_BT_END
		}

	private:
		boost::mutex m_self_lock;
		bool m_is_termed; // flags for other threads that we termed and may be blocked on zmq_term, m_context may be non null
		void* m_context;
	};


	struct socket_t {

        socket_t(context_t& context, int type){
        	GIE_DEBUG_TRACE();
        	m_s = zmq_socket (context.get(), type);
        	GIE_Z_THROW_IF( !m_s );
        }

        socket_t(socket_t&&other){
        	GIE_DEBUG_TRACE();
        	if(this!=&other){
        		this->m_s = other.m_s;
        		other.m_s = 0;
        	}
        }

		socket_t& operator=(socket_t&& other){
        	if(this!=&other){
        		this->close_();
        		this->m_s = other.m_s;
        		other.m_s = 0;
        	}
        	return *this;
		}


        ~socket_t(){
        	GIE_DEBUG_TRACE();
            close_();
        }


        void reset(){ this->close(); }
        void close_(){
            if(m_s){
				auto const rc = zmq_close (m_s);
				GIE_Z_THROW_IF( rc!=0 );
            }
        }
        void close(){
            if(m_s){
				auto const rc = zmq_close (m_s);
				GIE_Z_THROW_IF( rc!=0 );
				m_s = 0;
            }
        }

        void bind(char const*const endpoint){
        	assert(m_s);

        	GIE_Z_THROW_IF( zmq_bind(m_s, endpoint) );
        }

        void connect(char const*const endpoint){
        	assert(m_s);

        	GIE_Z_THROW_IF( zmq_connect(m_s, endpoint) );
        }

        inline
        void recv(message_t& message, int const flags = 0);
        inline
        void send(message_t& message, int const flags = 0);

        template<class T>
        void set_sock_opts(int const option, T const & value){
        	assert(m_s);

			zmq_retry(
				[&]{return zmq_setsockopt(m_s, option, &value, sizeof(value) );},
				[&](int rc){return rc;});
        }

        template<class T>
        void get_sock_opt(int const option, T & value){
        	size_t value_size = sizeof(value);

			zmq_retry(
				[&]()->int{
	        		auto const ret = zmq_getsockopt(m_s, option, &value, &value_size);
	        		GIE_CHECK( value_size == sizeof(value) );
	        		return ret;
				},
				[&](int rc){return rc;});

        }

        inline
        bool is_more(message_t& message);

        void set_linger(boost::posix_time::time_duration const& td){
			GIE_BT_BEGIN

			this->set_sock_opts(ZMQ_LINGER,  static_cast<int>( td.total_milliseconds() ) );

			GIE_BT_END
        }

		void* get()const{return m_s;}
	private:
		socket_t (socket_t const&);
		socket_t &operator=(socket_t const&);
	private:
		void *m_s;
	};



	template <class T>
	void std_delete_hint_op(void *data, void *hint){
		delete static_cast<T*>(hint);
	}

	template <class T>
	void std_delete_data_array_op(void *data, void *hint){
		delete[] static_cast<T*>(hint);
	}

	template <class VectorHolderWParent>
	void vector_with_parent_return_to_cache_deleter(void *data, void *hint){
		try{
			GIE_DEBUG_TRACE_INOUT();

			auto holder = static_cast<VectorHolderWParent*>(hint);
			holder->manager()->reuse(holder);
		}catch(...){
			assert(!"reuse() must not throw");
		}
	}

	namespace impl {
		struct message_fields_t {
			message_fields_t() : m_is_closed( false ) {}
			bool m_is_closed;
		};
	}

	struct message_t : impl::message_fields_t {

		message_t(){
        	GIE_DEBUG_TRACE_INOUT();
			GIE_Z_THROW_IF( zmq_msg_init(&m_msg) );
		}

		explicit
		message_t(size_t const size){
        	GIE_DEBUG_TRACE_INOUT();
			GIE_Z_THROW_IF( zmq_msg_init_size(&m_msg, size) );
		}

		message_t(char const*const data){
        	GIE_DEBUG_TRACE_INOUT();
        	assert(data);

        	auto const str_len = strlen(data);;
			GIE_Z_THROW_IF( zmq_msg_init_size(&m_msg, str_len ) );
			memcpy( this->data(), data, str_len );
		}

		void resize(size_t const size){
        	GIE_DEBUG_TRACE_INOUT();
			this->close();
			GIE_Z_THROW_IF( zmq_msg_init_size(&m_msg, size) );
		}

		message_t(message_t&& other){
        	GIE_DEBUG_TRACE_INOUT();
			GIE_Z_THROW_IF( zmq_msg_init(&m_msg) );
			try{
				GIE_Z_THROW_IF( zmq_msg_move(&m_msg, &other.m_msg) );
			}catch(...){
				this->close();
				throw;
			}
		}

		template<class Vector, class Deleter>
		explicit
		message_t(std::unique_ptr<Vector, Deleter>&& vec,
				typename std::enable_if< std::is_convertible<Vector, impl::holds_vector_with_parent_ref>::value >::type* = 0 ){

			GIE_DEBUG_TRACE_INOUT();
			GIE_Z_THROW_IF( zmq_msg_init_data(&m_msg, vec->data(), vec->size(), vector_with_parent_return_to_cache_deleter<Vector>, vec.get() ) );
			vec.release();

		}

		template<class Vector, class Deleter>
		void assign_data(std::unique_ptr<Vector, Deleter>&& vec,
				typename std::enable_if< std::is_convertible<Vector, impl::holds_vector_with_parent_ref>::value >::type* = 0 ){
			GIE_DEBUG_TRACE_INOUT();

			this->dispose_if_not_();

			try{
				GIE_Z_THROW_IF( zmq_msg_init_data(&m_msg, vec->data(), vec->size(), vector_with_parent_return_to_cache_deleter<Vector>, vec.get() ) );
			} catch(...) {
				this->m_is_closed=true;
			}

			vec.release();

		}

		template <class ElemType>
		void assign_data(ElemType const*const data, size_t const count){
			BOOST_STATIC_ASSERT( std::is_pod<ElemType>::value );
        	assert(data);

			this->dispose_if_not_();
			try{
	        	if(!count) {
	        		GIE_Z_THROW_IF( zmq_msg_init(&m_msg) );
	        	} else {
	    			auto const size = sizeof(ElemType)*count;
					GIE_Z_THROW_IF( zmq_msg_init_size(&m_msg, size ) );
	    			memcpy( this->data(), data, size );
	        	}
			} catch(...) {
				this->m_is_closed=true;
			}
		}

		void assign_data(char const*const data){
			GIE_DEBUG_TRACE_INOUT();
        	assert(data);

			this->dispose_if_not_();

        	auto const str_len = strlen(data);

			try{
				GIE_Z_THROW_IF( zmq_msg_init_size(&m_msg, str_len ) );
			} catch(...) {
				this->m_is_closed=true;
			}

			memcpy( this->data(), data, str_len );

		}

		void close(){
        	GIE_DEBUG_TRACE_INOUT();
        	if( !this->m_is_closed ){
        		this->dispose_();
    			this->m_is_closed=true;
        	}
		}

		void dispose_(){
			GIE_Z_THROW_IF( zmq_msg_close(&m_msg) );
		}

		void dispose_if_not_(){
			if( !this->m_is_closed ){
				this->dispose_();
			}
		}

		std::string to_string(){
			assert(!this->m_is_closed);

			return std::string( static_cast<char const*>(this->data()), this->size() );
		}


		~message_t(){
        	GIE_DEBUG_TRACE_INOUT();

        	try{
        		this->close();
        	}catch(...){
        		GIE_UNEXPECTED_IN_DTOR();
        	}
		}

		void const* data()const{
			return this->data_();
		}
		void * data(){
			return this->data_();
		}

		void* data_()const{
			assert( this->size()!=0 );

			auto const tmp=zmq_msg_data(&m_msg);
			GIE_Z_THROW_IF(!tmp);

			return tmp;
		}

		size_t size()const{
			return zmq_msg_size(&m_msg);
		}

		template<class ElemType>
		boost::iterator_range< typename boost::range_iterator<ElemType*>::type > range(){
			BOOST_STATIC_ASSERT( sizeof(ElemType)==1 );

			auto const begin = static_cast<ElemType*>( this->data() );
			return boost::make_iterator_range( begin, begin+this->size() );
		}


		zmq_msg_t const* get()const{ return &m_msg; }
		zmq_msg_t* get(){ return &m_msg; }
	private:
		zmq_msg_t mutable m_msg;
	};



    inline
    void socket_t::recv(message_t& message, int const flags){
    	GIE_BT_BEGIN

		zmq_retry(
			[&]{return zmq_recv(m_s, message.get(), flags);},
			[&](int rc){return rc;});

    	GIE_BT_END
    }


    inline
    void socket_t::send(message_t& message, int const flags){
    	assert(m_s);
    	GIE_BT_BEGIN

		zmq_retry(
			[&]{ return zmq_send(m_s, message.get(), flags); },
			[&](int rc){return rc;});

    	GIE_BT_END
    }

    inline
    bool socket_t::is_more(message_t& message){
    	GIE_BT_BEGIN

    	int64_t is_more_raw;
    	this->get_sock_opt(ZMQ_RCVMORE,is_more_raw);
    	return is_more_raw;

    	GIE_BT_END
    }

    template<typename... Args>
    socket_t context_t::socket(Args&&... args){
    	GIE_TMPL_DECLTYPE(m_self_lock)::scoped_lock lock(m_self_lock);

    	GIE_THROW_IF(m_is_termed, exception::term_e() );
    	assert(this->m_context);

    	socket_t new_socket(*this, std::forward<Args>(args)...);
    	return new_socket;
    }





} }
//================================================================================================================================================
#endif
//================================================================================================================================================
