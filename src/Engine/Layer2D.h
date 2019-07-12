#pragma once

#include "Kernel/Layer.h"
#include "Kernel/DummyRender.h"
#include "Kernel/DummyPicker.h"
#include "Kernel/ResourceImage.h"

#include "Kernel/Viewport.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    typedef IntrusivePtr<class RenderViewport> RenderViewportPtr;
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;

    class Layer2D
        : public Layer
        , public DummyRender
        , public DummyPicker
    {
        DECLARE_VISITABLE( Layer );
        DECLARE_RENDERABLE();
        DECLARE_PICKER();

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
        MENGINE_INLINE const RenderVertex2D * getVerticesImageMaskWM() const;
        MENGINE_INLINE const RenderMaterialInterfacePtr & getMaterialImageMask() const;

    protected:
        void updateVerticesImageMaskWM() const;
        void updateVerticesImageMaskColor() const;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;

    protected:
        mt::vec2f m_size;

        Viewport m_viewport;

        RenderCameraOrthogonalPtr m_renderCamera;
        RenderViewportPtr m_renderViewport;

        RenderInterfacePtr m_renderTarget;

        ResourceImagePtr m_resourceImageMask;

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
    MENGINE_INLINE const RenderVertex2D * Layer2D::getVerticesImageMaskWM() const
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
    MENGINE_INLINE const RenderMaterialInterfacePtr & Layer2D::getMaterialImageMask() const
    {
        return m_materialImageMask;
    }
}
