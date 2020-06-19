#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
    public:
        OpenGLRenderFragmentShader();
        ~OpenGLRenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory );

    public:
        bool compile();
        void release();

    public:
        void attach( GLuint _program );

    protected:
        ConstString m_name;

        MemoryInterfacePtr m_memory;

        GLuint m_shaderId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderFragmentShader> OpenGLRenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
