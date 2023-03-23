#include "OpenGLRenderVertexAttribute.h"
#include "OpenGLRenderEnum.h"
#include "OpenGLRenderErrorHelper.h"

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
    void OpenGLRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _index, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        AttributeDesc attr;
        attr.uniform = _uniform;
        attr.index = _index;
        attr.size = _size;
        attr.type = _type;
        attr.stride = _stride;
        attr.offset = _offset;
        attr.normalized = _normalized == true ? GL_TRUE : GL_FALSE;
        attr.location = -1;

        m_attributes.emplace_back( attr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::initialize( const ConstString & _name, uint32_t _elementSize )
    {
        m_name = _name;
        m_elementSize = _elementSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::finalize()
    {
        m_attributes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::compile( GLuint _program )
    {
        for( AttributeDesc & attribute : m_attributes )
        {
            const Char * attribute_uniform_str = attribute.uniform.c_str();

            int location;
            MENGINE_GLCALLR( location, glGetAttribLocation, (_program, attribute_uniform_str) );

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
        for( const AttributeDesc & attribute : m_attributes )
        {
            GLint attribute_location = attribute.location;

            MENGINE_GLCALL( glEnableVertexAttribArray, (attribute_location) );

            GLenum gl_type = Helper::toGLVertexAttributeType( attribute.type );

            MENGINE_GLCALL( glVertexAttribPointer, (attribute_location
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
        for( const AttributeDesc & attribute : m_attributes )
        {
            MENGINE_GLCALL( glDisableVertexAttribArray, (attribute.location) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}