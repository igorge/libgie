//================================================================================================================================================
// FILE: glwrap.h
// (c) GIE 2008-10-07  17:57
//
//================================================================================================================================================
#ifndef H_GUARD_GLWRAP_2008_10_07_17_57
#define H_GUARD_GLWRAP_2008_10_07_17_57
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "matrix_types.hpp"

#include <GL/gl.h>
//================================================================================================================================================
namespace gie { namespace gl {

    typedef gie::matrix44f matrix44f;

    struct exception
    {
        exception(GLenum const gl_error_code, char const * const file_name =0, char const * const func_name=0, unsigned int const line_no=0)
            : m_gl_error_code( gl_error_code )
            , m_file_name ( file_name )
            , m_func_name ( func_name )
            , m_line_no ( line_no )
        {}

        GLenum gl_error()const throw()
        {
            return m_gl_error_code;
        }

        private:
        GLenum const m_gl_error_code;
        char const * const m_file_name;
        char const * const m_func_name;
        unsigned int const m_line_no;
    };

    namespace impl {
        void raise_gl_exception(GLenum const gl_error_code, char const * const file_name =0, char const * const func_name=0, unsigned int const line_no=0)
        {
            assert(gl_error_code!=GL_NO_ERROR);
            throw ::gie::gl::exception(gl_error_code, file_name, func_name, line_no);
        }

        #define GIE_GL_CHECK_FOR_ERROR()\
            do {    \
                GLenum const gl_err_code=glGetError();  \
                if( gl_err_code!=GL_NO_ERROR )          \
                    ::gie::gl::impl::raise_gl_exception( gl_err_code, __FILE__, BOOST_CURRENT_FUNCTION, __LINE__);    \
            } while(false)              \
        /**/
    }


    inline
    void begin(GLenum const mode)
    {
        GIE_GL_CHECK_FOR_ERROR();
        glBegin(mode);
    }

    inline
    void end()
    {
        glEnd();
        GIE_GL_CHECK_FOR_ERROR();
    }

    inline
    void begin_triangles(){
        begin(GL_TRIANGLES);
    }

    inline
    void color(GLfloat const r, GLfloat const g, GLfloat const b)
    {
        glColor3f(r, g, b);
    }

    inline
    void vertex(GLfloat const x, GLfloat const y)
    {
        glVertex2f(x,y);
    }

    inline
    void vertex(GLfloat const x, GLfloat const y, GLfloat const z)
    {
        glVertex3f(x,y,z);
    }

    inline
    void tex_coord(GLfloat const u, GLfloat const v){
        glTexCoord2f(u, v);
    }

    inline
    void normal(GLfloat const nx, GLfloat const ny, GLfloat const nz){
        glNormal3f(nx, ny, nz);
    }

    inline
    void clear_buffer(GLbitfield const mask){
        glClear(mask);

        GIE_GL_CHECK_FOR_ERROR();
    }

    inline
    void clear_color_buffer(){
        return clear_buffer(GL_COLOR_BUFFER_BIT);
    }

    inline
    void clear_color_and_depth_buffer(){
        return clear_buffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    inline
    void get_model_view_matrix(matrix44f& m)
    {
        glGetFloatv(GL_MODELVIEW_MATRIX, m.data());
        GIE_GL_CHECK_FOR_ERROR();
    }
    
    typedef GLuint list_handle_t;
    list_handle_t const null_list = 0;

    inline
    list_handle_t gen_list(){
        auto const handle = glGenLists(1);
        GIE_GL_CHECK_FOR_ERROR();
        return handle;
    }

    inline
    void delete_list(list_handle_t const handle){
        assert(handle);
        glDeleteLists(handle, 1);
        GIE_GL_CHECK_FOR_ERROR();
    }

    inline
    void new_list(list_handle_t const handle){
        assert(handle);
        glNewList(handle, GL_COMPILE);
        GIE_GL_CHECK_FOR_ERROR();
    }

    inline
    void end_list(){
        glEndList();
        GIE_GL_CHECK_FOR_ERROR();
    }

} }
//================================================================================================================================================
#endif
//================================================================================================================================================
