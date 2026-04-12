#pragma once

#include "Interface/RenderSystemInterface.h"

#include "MetalRenderVertexShader.h"
#include "MetalRenderFragmentShader.h"
#include "MetalRenderVertexAttribute.h"
#include "MetalRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "Kernel/Vector.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MetalBlendStateKey
    {
        bool alphaBlend;
        EBlendFactor blendSrc;
        EBlendFactor blendDst;
        EBlendOp blendOp;
        EBlendFactor blendSeparateSrc;
        EBlendFactor blendSeparateDst;
        EBlendOp blendSeparateOp;
        bool blendSeparate;
        bool colorMaskR;
        bool colorMaskG;
        bool colorMaskB;
        bool colorMaskA;

        bool operator == ( const MetalBlendStateKey & _other ) const
        {
            return alphaBlend == _other.alphaBlend
                && blendSrc == _other.blendSrc
                && blendDst == _other.blendDst
                && blendOp == _other.blendOp
                && blendSeparateSrc == _other.blendSeparateSrc
                && blendSeparateDst == _other.blendSeparateDst
                && blendSeparateOp == _other.blendSeparateOp
                && blendSeparate == _other.blendSeparate
                && colorMaskR == _other.colorMaskR
                && colorMaskG == _other.colorMaskG
                && colorMaskB == _other.colorMaskB
                && colorMaskA == _other.colorMaskA;
        }

        bool operator != ( const MetalBlendStateKey & _other ) const
        {
            return !(*this == _other);
        }
    };
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
        const MetalRenderVertexShaderPtr & getMetalVertexShader() const;
        const MetalRenderFragmentShaderPtr & getMetalFragmentShader() const;

    public:
        id<MTLRenderPipelineState> getPipelineState() const;
        id<MTLRenderPipelineState> getOrCreatePipelineState( const MetalBlendStateKey & _key );

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

        struct PipelineStateCacheEntry
        {
            MetalBlendStateKey key;
            id<MTLRenderPipelineState> pipelineState;
        };

        typedef Vector<PipelineStateCacheEntry> VectorPipelineStateCache;
        VectorPipelineStateCache m_pipelineStateCache;

        bool m_deferredCompile;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderProgram, RenderProgramInterface> MetalRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}
