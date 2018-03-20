#   pragma once

#   include "Interface/RenderSystemInterface.h"

#   include "OpenGLRenderHeader.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderFragmentShader
        : public RenderFragmentShaderInterface
    {
    public:
        OpenGLRenderFragmentShader();
        ~OpenGLRenderFragmentShader();

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory );
        bool compile();

    public:
        void attach( GLuint _program );

    protected:
        GLuint createShader_( GLenum type, const char * _source );

    protected:
        ConstString m_name;
        MemoryInterfacePtr m_memory;

        GLuint m_shaderId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderFragmentShader> OpenGLRenderFragmentShaderPtr;
}	// namespace Menge
