#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "RigidBody2D.h"
#	include "Tile.h"

namespace Menge
{
	class ResourceImage;
	class RenderImageInterface;
	class ResourceTilePolygon;

	class TilePolygon
		: public RigidBody2D
	{
		OBJECT_DECLARE(TilePolygon)

	public:
		TilePolygon();

	public:
		void addVertex( const mt::vec2f & _vertex );

	public:
		void loader( XmlElement * _xml ) override;
		void _loaderVertices( XmlElement * _xml );
		void _render( unsigned int _debugMask ) override;

	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:

		std::list<Tile> m_tiles;

		ResourceTilePolygon * m_tilePolygonResource;
		const RenderImageInterface * m_image;
		const RenderImageInterface * m_imageJunc;


		String m_resourcename;
		String m_juncName;

		String m_tileResource;

		mt::TVectorPoints m_poly;
		mt::TVectorPoints m_triangles;
		mt::TVectorPoints m_uvs;

		ResourceImage * m_resource;
		ResourceImage * m_resourceJunc;
	};
};