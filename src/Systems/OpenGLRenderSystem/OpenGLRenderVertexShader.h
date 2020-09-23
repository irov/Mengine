#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/Factorable.h"
#include "Kernel/CompilableReference.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderVertexShader
        : public RenderVertexShaderInterface        
        , public Factorable
        , public CompilableReference
    {
    public:
        OpenGLRenderVertexShader();
        ~OpenGLRenderVertexShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory );
        void finalize();

    public:
        bool _compile() override;
        void _release() override;

    public:
        void attach( GLuint _program );

    protected:
        ConstString m_name;

        MemoryInterfacePtr m_memory;

        GLuint m_shaderId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderVertexShader> OpenGLRenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
