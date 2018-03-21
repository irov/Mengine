#include "OpenGLRenderVertexAttribute.h"

#include "OpenGLRenderEnum.h"
#include "OpenGLRenderError.h"

#include "Logger/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexAttribute::OpenGLRenderVertexAttribute()
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
        uint32_t vertexSize = 0;

        for( TVectorAttribute::const_iterator
            it = m_attributes.begin(),
            it_end = m_attributes.end();
            it != it_end;
            ++it )
        {
            const Attribute & attribute = *it;

            switch( attribute.type )
            {
            case VAT_FLOAT:
                {
                    vertexSize += attribute.size * sizeof( float );
                }break;
            case VAT_UNSIGNED_BYTE:
                {
                    vertexSize += attribute.size * sizeof( unsigned char );
                }break;
            }            
        }

        return vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset )
    {
        Attribute attr;
        attr.uniform = _uniform;
        attr.size = _size;
        attr.type = _type;
        attr.normalized = _normalized;
        attr.stride = _stride;
        attr.offset = _offset;

        m_attributes.push_back( attr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::initialize( const ConstString & _name )
    {
        m_name = _name;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexAttribute::enable()
    {
        for( uint32_t
            it = 0,
            it_end = m_attributes.size();
            it != it_end;
            ++it )
        {
            const Attribute & attribute = m_attributes[it];

            GLCALL( glEnableVertexAttribArray, (it) );

            GLenum gl_type = s_getGLVertexAttributeType( attribute.type );

            GLCALL( glVertexAttribPointer, (it, attribute.size, gl_type, static_cast<GLboolean>(attribute.normalized), attribute.stride, reinterpret_cast<const GLvoid *>(attribute.offset)) );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::disable()
    {
        for( uint32_t
            it = 0,
            it_end = m_attributes.size();
            it != it_end;
            ++it )
        {
            GLCALL( glDisableVertexAttribArray, (it) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexAttribute::bind( GLuint _program )
    {
        for( uint32_t
            it = 0,
            it_end = m_attributes.size();
            it != it_end;
            ++it )
        {
            const Attribute & attribute = m_attributes[it];

            const char * uniform_str = attribute.uniform.c_str();

            GLCALL( glBindAttribLocation, (_program, it, uniform_str) );
        }
    }
}