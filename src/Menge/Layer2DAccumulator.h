#	pragma once

#	include "Layer2D.h"


namespace Menge
{
	class RenderImageInterface;
	class Layer2DAccumulator 
		: public Layer2D
	{
		OBJECT_DECLARE( Layer2DAccumulator );
	public:
		void render( const Viewport & _viewport, bool _enableDebug ) override;
		void _render( const Viewport & _viewport, bool _enableDebug ) override;

	public:
		struct ImageRect 
		{
			mt::box2f rect;
			RenderImageInterface* image;
		};
		typedef std::vector<ImageRect> TRenderImageVector;

	protected:
		bool _compile() override;
		void _release() override;

		TRenderImageVector m_surfaces;
	};
}	// namespace Menge