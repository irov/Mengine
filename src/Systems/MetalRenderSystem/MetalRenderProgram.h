#pragma once

#include "Interface/RenderProgramInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class MetalRenderProgram
        : public RenderProgramInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderProgram );

    public:
        MetalRenderProgram();
        ~MetalRenderProgram() override;

    public:
        bool initialize( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _attribute, uint32_t _samplerCount );
        void finalize();

    public:
        const ConstString & getName() const override;
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;

    protected:
        ConstString m_name;
        RenderVertexShaderInterfacePtr m_vertexShader;
        RenderFragmentShaderInterfacePtr m_fragmentShader;
        RenderVertexAttributeInterfacePtr m_vertexAttribute;
    };

    typedef IntrusivePtr<MetalRenderProgram, RenderProgramInterface> MetalRenderProgramPtr;
}

