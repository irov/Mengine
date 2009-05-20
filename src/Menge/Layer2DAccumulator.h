#	pragma once

#	include "Layer2D.h"


namespace Menge
{
	class Texture;
	class Layer2DAccumulator 
		: public Layer2D
	{
		OBJECT_DECLARE( Layer2DAccumulator );
	public:
		Layer2DAccumulator();

		void loader( XmlElement * _xml ) override;

		void render( unsigned int _debugMask ) override;
		void _render( unsigned int _debugMask ) override;

	public:
		struct ImageRect 
		{
			mt::box2f rect;
			Texture* image;
			Camera2D* camera;
		};
		typedef std::vector<ImageRect> TRenderImageVector;

	protected:
		bool _compile() override;
		void _release() override;

		float m_gridSize;
		TRenderImageVector m_surfaces;
		//std::vector<RenderObject*> m_ros;
	};
}	// namespace Menge
