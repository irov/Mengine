#	pragma once

#	include "Kernel/Layer.h"

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

    protected:
        bool _activate() override;
        void _deactivate() override;

	protected:
        mt::vec2f m_size;

		Viewport m_viewport;
		
		RenderCameraOrthogonal * m_renderCamera;
		RenderViewport * m_renderViewport;

		bool m_hasViewport;
	};
}
