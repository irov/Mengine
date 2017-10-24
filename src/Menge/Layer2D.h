#	pragma once

#	include "Kernel/Layer.h"
#   include "Kernel/ResourceImage.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class RenderCameraOrthogonal;
	class RenderViewport;

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
        void setImageMask( const ResourceImagePtr & _resourceImageMask );
        void removeImageMask();

    protected:
        void createRenderTarget_();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void _renderTarget( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;

	protected:
        mt::vec2f m_size;

		Viewport m_viewport;
		
		RenderCameraOrthogonal * m_renderCamera;
		RenderViewport * m_renderViewport;

        ResourceImagePtr m_resourceImageMask;
        //RenderTargetInterfacePtr m_renderTarget;
        //RenderImageInterfacePtr m_renderTargetImage;
        //RenderTextureInterfacePtr m_renderTargetTexture;

        RenderMaterialInterfacePtr m_materialImageMask;

        RenderVertex2D m_verticesImageMaskWM[4];

		bool m_hasViewport;
        bool m_hasImageMask;
	};
}
