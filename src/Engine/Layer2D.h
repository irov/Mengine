#pragma once

#include "Kernel/Layer.h"
#include "Kernel/ResourceImage.h"

#include "Core/Viewport.h"
#include "Core/RenderVertex2D.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    typedef IntrusivePtr<class RenderViewport> RenderViewportPtr;
    typedef IntrusivePtr<class RenderMaterialInterface> RenderMaterialInterfacePtr;

	class Layer2D
		: public Layer
	{
		DECLARE_VISITABLE( Layer )

	public:
		Layer2D();

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
        void _renderTarget( const RenderContext * _state ) override;

    protected:
        inline const RenderVertex2D * getVerticesImageMaskWM() const;

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

        ResourceHolder<ResourceImage> m_resourceImageMask;
        //RenderTargetInterfacePtr m_renderTarget;
        //RenderImageInterfacePtr m_renderTargetImage;
        //RenderTextureInterfacePtr m_renderTargetTexture;

        RenderMaterialInterfacePtr m_materialImageMask;

        mutable RenderVertex2D m_verticesImageMaskWM[4];
        mutable bool m_invalidateVerticesImageMaskColor;
        mutable bool m_invalidateVerticesImageMaskWM;

		bool m_hasViewport;
        bool m_hasImageMask;
	};
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
}
