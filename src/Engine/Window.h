#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/FixedVertices.h"

#include "Engine/ResourceWindow.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Window
        : public Node
        , public FixedVertices<ResourceWindow_Count * 4>
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Window );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Window();
        ~Window() override;

    public:
        void setClientSize( const mt::vec2f & _clientSize );
        void setClientSizeClip( const mt::vec2f & _clientSize );
        void setClientSizeInTiles( const mt::vec2f & _tiles );

        const mt::vec2f & getClientSize() const;

    public:
        mt::vec2f getWindowSize() const;
        const mt::vec2f getTileSize( uint32_t _tile ) const;

        void setResourceWindow( const ResourceWindowPtr & _resourceWindow );
        const ResourceWindowPtr & getResourceWindow() const;

        bool hasBackground() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;
        void _invalidateWorldMatrix() const override;
        void _invalidateColor() const override;

    protected:
        void _dispose() override;

    protected:
        void _updateVertices( RenderVertex2D * const _vertices, uint8_t _invalidateVertices ) const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        ResourceWindowPtr m_resourceWindow;

        mt::vec2f m_clientSize;

        struct WindowEdge
        {
            mt::vec2f initialSize = {0.f, 0.f};
            RenderMaterialInterfacePtr material;
        };

        WindowEdge m_edge[ResourceWindow_Count];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Window> WindowPtr;
    //////////////////////////////////////////////////////////////////////////
}
