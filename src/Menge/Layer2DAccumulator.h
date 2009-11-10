#	pragma once

#	include "Layer2D.h"

namespace Menge
{
	class Texture;
	class Layer2DAccumulator
		: public Layer2D
	{
		FACTORABLE_DECLARE( Layer2DAccumulator );
	public:
		Layer2DAccumulator();

		void loader( XmlElement * _xml ) override;

		void render( Camera2D * _camera ) override;
		void _render( Camera2D * _camera ) override;

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
		typedef std::vector<Material*> TMaterialVector;
		TMaterialVector m_materials;
		TVertex2DVector m_vertices;
	};
}	// namespace Menge
