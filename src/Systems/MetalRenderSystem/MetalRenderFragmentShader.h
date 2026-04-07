#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderFragmentShader
        : public RenderFragmentShaderInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderFragmentShader );

    public:
        MetalRenderFragmentShader();
        ~MetalRenderFragmentShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompiled );
        void finalize();

    public:
        bool compile();
        void release();
        bool isCompile() const;

    public:
        id<MTLFunction> getFunction() const;

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ConstString m_name;
        MemoryInterfacePtr m_memory;

        id<MTLFunction> m_function;
        id<MTLLibrary> m_library;

        bool m_compile;
        bool m_precompiled;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderFragmentShader, RenderFragmentShaderInterface> MetalRenderFragmentShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
