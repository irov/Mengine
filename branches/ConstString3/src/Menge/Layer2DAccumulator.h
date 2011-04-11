//#	pragma once
//
//#	include "Layer2D.h"
//
//namespace Menge
//{
//	class Texture;
//	class Layer2DAccumulator
//		: public Layer2D
//	{
//	public:
//		Layer2DAccumulator();
//
//		void loader( BinParser * _parser ) override;
//
//		void render( Camera2D * _camera ) override;
//		void _render( Camera2D * _camera ) override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//	protected:
//		struct ImageRect
//		{
//			mt::box2f rect;
//			const Texture * image;
//			Camera2D * camera;
//		};
//
//		typedef std::vector<ImageRect> TVectorRenderImage;
//
//		float m_gridSize;
//		TVectorRenderImage m_surfaces;
//
//		typedef std::vector<const Material*> TVectorMaterial;
//		TVectorMaterial m_materials;
//		TVectorVertex2D m_vertices;
//
//		class VisitorRenderLayer2DPool;
//	};
//}	// namespace Menge
