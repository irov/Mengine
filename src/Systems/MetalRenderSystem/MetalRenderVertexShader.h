#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/MemoryInterface.h"

#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"
#include "Kernel/ReferenceCounter.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderVertexShader
        : public RenderVertexShaderInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexShader );

    public:
        MetalRenderVertexShader();
        ~MetalRenderVertexShader() override;

    public:
        const ConstString & getName() const override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _precompiled );
        void finalize();

    public:
        bool compile();
        void release();

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

        ReferenceCounter m_compileReferenceCount;

        bool m_precompiled;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderVertexShader, RenderVertexShaderInterface> MetalRenderVertexShaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
