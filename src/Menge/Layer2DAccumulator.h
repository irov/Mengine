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
		Layer2DAccumulator();

		void loader( XmlElement * _xml ) override;

		void render( bool _enableDebug ) override;
		void _render( bool _enableDebug ) override;

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

		int m_gridSize;
		TRenderImageVector m_surfaces;
	};
}	// namespace Menge