//================================================================================================================================================
// FILE: sd_util_qio2stream.h
// (c) GIE 2010-06-14  12:46
//
//================================================================================================================================================
#ifndef H_GUARD_SD_UTIL_QIO2STREAM_2010_06_14_12_46
#define H_GUARD_SD_UTIL_QIO2STREAM_2010_06_14_12_46
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "../exceptions.hpp"

#include <QtCore/QIODevice>
#include <iostream>
//================================================================================================================================================
namespace gie {

    namespace exception {

        struct qio_device_open_failed : virtual root {};
    }

    namespace impl  {


        template <class SelfT, class Parent>
        struct dummy_istream_to_qio_t : Parent
        {
            virtual qint64 readData  ( char * /*data*/, qint64 /*maxSize*/ ){
                return -1;
            }

        };
        template <class SelfT, class Parent>
        struct dummy_ostream_to_qio_t : Parent
        {
            virtual qint64 writeData  ( char const * /*data*/, qint64 /*maxSize*/ ){
                return -1;
            }

        };

        template <class SelfT, class Parent>
        struct ostream_to_qio_t : Parent
        {

            virtual qint64 writeData  ( const char * data, qint64 maxSize ){
                if(!get_os().good()) return -1;

                get_os().write( data, maxSize );
                if(get_os().bad()) return -1;
                return maxSize;
            }

        private:
            std::ostream& get_os(){
                return static_cast<SelfT*>(this)->get_os();
            }
        };

        template <class SelfT, class Parent>
        struct istream_to_qio_t : Parent
        {
            
            virtual qint64 readData  ( char * data, qint64 maxSize ){
                if(!get_is().good()) return -1;

                get_is().read( data, maxSize );
                std::streamsize const gcount = get_is().gcount();

                if(get_is().bad()) return -1;

                return gcount;
            }


            virtual bool isSequential() const{
                return true;
            }

        private:
            std::istream& get_is(){
                return static_cast<SelfT*>(this)->get_is();
            }

        };


    } // end imp ns


    struct ostream_to_qio_t
        : impl::ostream_to_qio_t<ostream_to_qio_t, impl::dummy_istream_to_qio_t<ostream_to_qio_t, QIODevice> >
    {
        friend struct impl::ostream_to_qio_t<ostream_to_qio_t, impl::dummy_istream_to_qio_t<ostream_to_qio_t, QIODevice> >;
        
        ostream_to_qio_t(std::ostream& os)
            : m_os ( os )
        {
            if( !this->open(QIODevice::WriteOnly) ){
                GIE_THROW( exception::qio_device_open_failed() );
            }
        }
    protected:
        std::ostream& get_os(){
            return m_os;
        }

    private:
        std::ostream& m_os;
    };


    struct istream_to_qio_t
        : impl::istream_to_qio_t<istream_to_qio_t, impl::dummy_ostream_to_qio_t<istream_to_qio_t, QIODevice> >
    {
        friend struct impl::istream_to_qio_t<istream_to_qio_t, impl::dummy_ostream_to_qio_t<istream_to_qio_t, QIODevice> >;

        istream_to_qio_t(std::istream& os)
            : m_os ( os )
        {
            if( !this->open(QIODevice::ReadOnly) ){
                GIE_THROW( exception::qio_device_open_failed() );
            }
        }
    protected:
        std::istream& get_is(){
            return m_os;
        }

    private:
        std::istream& m_os;
    };


}
//================================================================================================================================================
#endif
//================================================================================================================================================
