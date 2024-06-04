#pragma once

#include "Kernel/Layer.h"
#include "Kernel/DummyRender.h"
#include "Kernel/DummyPicker.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderViewport.h"

#include "Kernel/Viewport.h"
#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Layer2D
        : public Layer
        , protected DummyRender
        , protected DummyPicker
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Layer2D );
        DECLARE_VISITABLE( Layer );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Layer2D();
        ~Layer2D() override;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

    public:
        void setViewport( const Viewport & _viewport );
        bool getViewport( Viewport * const _viewport ) const;
        void removeViewport();

    protected:
        void createViewport_();
        void clearViewport_();

    public:
        bool setImageMask( const ResourceImagePtr & _resourceImageMask );
        bool getImageMask( ResourceImagePtr * const _resourceImageMask ) const;
        void removeImageMask();

    protected:
        bool createRenderTarget_();
        void clearRenderTarget_();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void _dispose() override;

    protected:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;

    public:
        MENGINE_INLINE const RenderVertex2D * getVerticesImageMaskWM() const;
        MENGINE_INLINE const RenderMaterialInterfacePtr & getMaterialImageMask() const;

    protected:
        void updateVerticesImageMaskWM() const;
        void updateVerticesImageMaskColor() const;

    protected:
        void _invalidateColor() const override;
        void _invalidateWorldMatrix() const override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
        const RenderScissorInterfacePtr & getPickerScissor() const override;
        const RenderTransformationInterfacePtr & getPickerTransformation() const override;
        const RenderTargetInterfacePtr & getPickerTarget() const override;

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
    typedef IntrusiveNodePtr<Layer2D> Layer2DPtr;
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
    //////////////////////////////////////////////////////////////////////////
}
