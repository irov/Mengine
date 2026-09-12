#pragma once

#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderResolution.h"
#include "Kernel/Viewport.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectViewerExamplePreviewCanvas
        : public Node
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( FontEffectViewerExamplePreviewCanvas );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        FontEffectViewerExamplePreviewCanvas();
        ~FontEffectViewerExamplePreviewCanvas() override;

    public:
        bool setSize( uint32_t _width, uint32_t _height );
        const mt::vec2f & getSize() const;

    public:
        void setZoom( float _zoom );
        float getZoom() const;

    public:
        const RenderTargetInterfacePtr & getTarget() const;
        const RenderTextureInterfacePtr & getTexture() const;

    public:
        const RenderMaterialInterfacePtr & getCompositeMaterial() const;
        const RenderVertex2D * getCompositeVertices() const;
        const RenderIndex * getCompositeIndices() const;

    protected:
        bool _activate() override;
        void _deactivate() override;
        void _dispose() override;

    protected:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool createRenderTarget_();
        void clearRenderTarget_();
        void updateCamera_();

    protected:
        mt::vec2f m_size;
        float m_zoom;

        RenderTargetInterfacePtr m_sceneTarget;
        RenderTextureInterfacePtr m_sceneTexture;
        RenderMaterialInterfacePtr m_compositeMaterial;
        RenderInterfacePtr m_compositeRender;

        RenderCameraOrthogonalPtr m_renderCamera;
        RenderViewportPtr m_renderViewport;
        RenderResolutionPtr m_renderResolution;

        RenderVertex2D m_compositeVertices[4];
        RenderIndex m_compositeIndices[6] = {0, 1, 2, 0, 2, 3};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<FontEffectViewerExamplePreviewCanvas> FontEffectViewerExamplePreviewCanvasPtr;
    //////////////////////////////////////////////////////////////////////////
}
