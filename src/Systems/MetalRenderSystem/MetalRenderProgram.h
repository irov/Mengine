#pragma once

#include "Interface/RenderSystemInterface.h"

#include "MetalRenderVertexShader.h"
#include "MetalRenderFragmentShader.h"
#include "MetalRenderVertexAttribute.h"
#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderProgram
        : public RenderProgramInterface
        , public MetalRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderProgram );

    public:
        MetalRenderProgram();
        ~MetalRenderProgram() override;

    public:
        const ConstString & getName() const override;

    public:
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;

    public:
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;

    public:
        bool initialize( const ConstString & _name, const MetalRenderVertexShaderPtr & _vertexShader, const MetalRenderFragmentShaderPtr & _fragmentShader, const MetalRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount );
        void finalize();

    public:
        void setDeferredCompile( bool _deferredCompile );
        bool getDeferredCompile() const;

    public:
        bool compile();
        void release();

    public:
        uint32_t getSamplerCount() const;

    public:
        const MetalRenderVertexAttributePtr & getMetalVertexAttribute() const;

    public:
        id<MTLRenderPipelineState> getPipelineState() const;

    public:
        void bindMatrix( id<MTLRenderCommandEncoder> _encoder, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalWVPMatrix ) const;

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ConstString m_name;

        MetalRenderVertexShaderPtr m_vertexShader;
        MetalRenderFragmentShaderPtr m_fragmentShader;
        MetalRenderVertexAttributePtr m_vertexAttribute;

        uint32_t m_samplerCount;

        id<MTLRenderPipelineState> m_pipelineState;

        bool m_deferredCompile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderProgram, RenderProgramInterface> MetalRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}
