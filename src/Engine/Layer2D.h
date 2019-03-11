#pragma once

#include "Kernel/Layer.h"
#include "Kernel/NoneRender.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceHolder.h"

#include "Kernel/Viewport.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    typedef IntrusivePtr<class RenderViewport> RenderViewportPtr;
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;

    class Layer2D
        : public Layer
        , public NoneRender
    {
        DECLARE_VISITABLE( Layer );
        DECLARE_RENDERABLE();

    public:
        Layer2D();
        ~Layer2D() override;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

    public:
        void setViewport( const Viewport & _viewport );
        void removeViewport();

    protected:
        void createViewport_();
        void clearViewport_();

    public:
        bool setImageMask( const ResourceImagePtr & _resourceImageMask );
        void removeImageMask();

    protected:
        bool createRenderTarget_();
        void clearRenderTarget_();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;

    public:
        inline const RenderVertex2D * getVerticesImageMaskWM() const;
        inline const RenderMaterialInterfacePtr & getMaterialImageMask() const;

    protected:
        void updateVerticesImageMaskWM() const;
        void updateVerticesImageMaskColor() const;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        mt::vec2f m_size;

        Viewport m_viewport;

        RenderCameraOrthogonalPtr m_renderCamera;
        RenderViewportPtr m_renderViewport;

        RenderInterfacePtr m_renderTarget;

        ResourceHolder<ResourceImage> m_resourceImageMask;

        RenderMaterialInterfacePtr m_materialImageMask;

        mutable RenderVertex2D m_verticesImageMaskWM[4];
        mutable bool m_invalidateVerticesImageMaskColor;
        mutable bool m_invalidateVerticesImageMaskWM;

        bool m_hasViewport;
        bool m_hasImageMask;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Layer2D> Layer2DPtr;
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * Layer2D::getVerticesImageMaskWM() const
    {
        if( m_invalidateVerticesImageMaskColor == true )
        {
            m_invalidateVerticesImageMaskColor = false;

            this->updateVerticesImageMaskColor();
        }

        if( m_invalidateVerticesImageMaskWM == true )
        {
            m_invalidateVerticesImageMaskWM = false;

            this->updateVerticesImageMaskWM();
        }

        return m_verticesImageMaskWM;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterialInterfacePtr & Layer2D::getMaterialImageMask() const
    {
        return m_materialImageMask;
    }
}
