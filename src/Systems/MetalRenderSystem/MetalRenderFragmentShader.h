#pragma once

#include "Interface/RenderFragmentShaderInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetalRenderFragmentShader
        : public RenderFragmentShaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderFragmentShader );

    public:
        MetalRenderFragmentShader();
        ~MetalRenderFragmentShader() override;

    public:
        bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile );
        void finalize();

    public:
        const ConstString & getName() const override;

    protected:
        ConstString m_name;
        MemoryInterfacePtr m_memory;
    };

    typedef IntrusivePtr<MetalRenderFragmentShader, RenderFragmentShaderInterface> MetalRenderFragmentShaderPtr;
}

