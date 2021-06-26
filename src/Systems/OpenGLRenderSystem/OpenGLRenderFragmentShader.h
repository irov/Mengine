#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"
#include "Kernel/CompilableReference.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderFragmentShader
        : public RenderFragmentShaderInterface
        , public OpenGLRenderResourceHandler
        , public CompilableReference
        , public Factorable
    {
        DECLARE_FACTORABLE( RenderImageInterface );

    public:
        OpenGLRenderFragmentShader();
        ~OpenGLRenderFragmentShader() override;

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

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ConstString m_name;

        MemoryInterfacePtr m_memory;

        GLuint m_shaderId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderFragmentShader, RenderFragmentShaderInterface> OpenGLRenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
