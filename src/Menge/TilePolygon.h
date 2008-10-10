#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "RigidBody2D.h"
#	include "ResourceTilePolygon.h"

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
		void loader( XmlElement * _xml ) override;
		void _loaderVertices( XmlElement * _xml );
		void _render( unsigned int _debugMask ) override;
		void addVertex( const mt::vec2f& _vertex );

	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:

		void updateTileBoundingBox(mt::box2f& _boundingBox, const std::vector<Quad>& quads);

		std::vector<const std::vector<Quad>*> m_tileGeometry;
		std::vector<const std::vector<Quad>*> m_junkGeometry;

		struct TQuad
		{
			mt::vec2f a,b,c,d;
			float s,t;
		};
		typedef std::vector<TQuad> TVectorQuad;
		typedef std::map< const RenderImageInterface*, TVectorQuad > TQuadMap;
		TQuadMap m_edges;		// quads on edges with texture
		TQuadMap m_edge_juncs;	// quads between edges with same texture
		TVectorQuad m_juncs;	// quads between edges with different texture

		ResourceTilePolygon * m_tilePolygonResource;
		String m_tileResource;

		mt::TVectorPoints m_triangles;
		mt::TVectorPoints m_uvs;
		mt::TVectorPoints m_poly;
		const RenderImageInterface* m_image;
		const RenderImageInterface* m_junc_image;

		void proccessEdges_();
		const ResourceTilePolygon::TileDecl* getNextTileDecl_( const ResourceTilePolygon::TTileDecls& _decls, int _i );
	};
};