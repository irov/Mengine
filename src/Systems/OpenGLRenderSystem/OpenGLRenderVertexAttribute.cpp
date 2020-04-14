#include "OpenGLRenderVertexAttribute.h"

#include "OpenGLRenderEnum.h"
#include "OpenGLRenderError.h"

#include "Kernel/Logger.h"

#include "Config/UInt32ToPointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexAttribute::OpenGLRenderVertexAttribute()
        : m_elementSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexAttribute::~OpenGLRenderVertexAttribute()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderVertexAttribute::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderVertexAttribute::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        Attribute attr;
        attr.uniform = _uniform;
        attr.location = -1;
        attr.size = _size;
        attr.type = _type;
        attr.normalized = _normalized == true ? GL_TRUE : GL_FALSE;
        attr.stride = _stride;
        attr.offset = _offset;

        m_attributes.push_back( attr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::compile( GLuint _program )
    {
        for( Attribute & attribute : m_attributes )
        {
            const Char * attribute_uniform_str = attribute.uniform.c_str();

            int location;
            GLCALLR( location, glGetAttribLocation, (_program, attribute_uniform_str) );

            if( location == -1 )
            {
                LOGGER_ERROR( "vertex attribute '%s' invalid get uniform '%s'"
                    , m_name.c_str()
                    , attribute.uniform.c_str()
                );

                return false;
            }

            attribute.location = location;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::enable()
    {
        for( const Attribute & attribute : m_attributes )
        {
            GLint attribute_location = attribute.location;

            GLCALL( glEnableVertexAttribArray, (attribute_location) );

            GLenum gl_type = s_getGLVertexAttributeType( attribute.type );

            GLCALL( glVertexAttribPointer, (attribute_location
                , attribute.size
                , gl_type
                , attribute.normalized
                , attribute.stride
                , MENGINE_UINT32_TO_POINTER( const GLvoid *, attribute.offset )
                ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::disable()
    {
        for( const Attribute & attribute : m_attributes )
        {
            GLCALL( glDisableVertexAttribArray, (attribute.location) );
        }
    }
}