#pragma once

#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetalRenderVertexShader
        : public RenderVertexShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderVertexShader );

    public:
        MetalRenderVertexShader();
        ~MetalRenderVertexShader() override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    public:
        const ConstString & getName() const override;

    protected:
        ConstString m_name;
        MemoryInterfacePtr m_memory;
    };

    typedef IntrusivePtr<MetalRenderVertexShader, RenderVertexShaderInterface> MetalRenderVertexShaderPtr;
}

