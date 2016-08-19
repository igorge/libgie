//================================================================================================================================================
// FILE: sdlwrap.h
// (c) GIE 2008-10-06  13:49
//
//================================================================================================================================================
#ifndef H_GUARD_SDLWRAP_2008_10_06_13_49
#define H_GUARD_SDLWRAP_2008_10_06_13_49
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "gie/exceptions.hpp"

#include <SDL.h>

#include <boost/exception/all.hpp>
#include <boost/noncopyable.hpp>
#include <boost/utility/singleton.hpp>

//================================================================================================================================================
namespace gie { namespace sdl {


    namespace exception {

        typedef boost::error_info<struct tag_error_sdl,std::string> error_sdl;
        typedef boost::error_info<struct tag_error_sdl_init_flags,unsigned int> error_sdl_init_flags;

        struct root : virtual ::gie::exception::root {};

        struct init_failed : virtual root {};
        struct non_initialized : virtual root {};
        
        struct video_info_query_failed : virtual root {};
        struct video_mode_set_failed : virtual root {};
        struct gl_attr_set_failed : virtual root {};
        struct video_already_initialized : virtual init_failed {};
    }

    struct service : boost::singleton<service>
    {
        typedef SDL_Event event_type;

        std::string get_error(){
            char const*const sdl_err = SDL_GetError();

            if(!sdl_err){
                GIE_UNEXPECTED();
            }

            return sdl_err;
        }

        service(boost::restricted){

            if( SDL_Init( 0 )!=0 ) {
                GIE_THROW( exception::init_failed() << exception::error_sdl( SDL_GetError() ) << exception::error_sdl_init_flags(0) );
            }

        }


        /*service(unsigned int const init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO)
        {
            if( SDL_Init( init_flags ) <0 ) {
                GIE_THROW( exception::init_failed() << exception::error_sdl( SDL_GetError() ) << exception::error_sdl_init_flags(init_flags) );
            }
        }*/

        void  initialize_video(){
            if( is_video_initialized() ) {
                GIE_THROW( exception::video_already_initialized() );
            }

            if( SDL_InitSubSystem( SDL_INIT_VIDEO ) !=0 ) {
                GIE_THROW( exception::init_failed() << exception::error_sdl( SDL_GetError() ) << exception::error_sdl_init_flags(SDL_INIT_VIDEO) );
            }
        }

        service& set_gl_attr(SDL_GLattr const attr, int const value){

            if( SDL_GL_SetAttribute(attr, value)!=0 ){
                GIE_THROW( exception::gl_attr_set_failed() << exception::error_sdl( get_error() ) );
            }

            return *this;
        }

        bool is_video_initialized()const{
            return (SDL_WasInit(SDL_INIT_VIDEO)==SDL_INIT_VIDEO);
        }

        SDL_VideoInfo const& video_info()
        {
            if( !is_video_initialized() ) {
                GIE_THROW( exception::non_initialized() << exception::error_sdl_init_flags(SDL_INIT_VIDEO) );
            }

            SDL_VideoInfo const*const video_info = SDL_GetVideoInfo();
            if( !video_info ) {
                GIE_THROW( exception::video_info_query_failed() << exception::error_sdl( get_error() ) );
            }
            
            return *video_info;
        }

        SDL_Surface * set_video_mode(unsigned int const width, unsigned int const height, unsigned int const bpp, Uint32 const flags){

            SDL_Surface * const surface = SDL_SetVideoMode( width, height, bpp, flags );
            if( !surface ){
                GIE_THROW( exception::video_mode_set_failed() << exception::error_sdl( get_error() ) );
            }

            return surface;

        }

        bool poll_event(event_type & event){
            return SDL_PollEvent(&event)!=0;
        }

        void gl_swap_buffers(){
            SDL_GL_SwapBuffers();
        }

        ~service()
        {
            SDL_Quit();
            printf("SDL shutting down");
        }
    };


    //struct basic_surface



} }
//================================================================================================================================================
#endif
//================================================================================================================================================
