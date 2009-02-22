#	pragma once
#	include "math/vec2.h"
#	include "math/polygon.h"
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
		void _invalidateWorldMatrix() override;

	private:

		struct Quad //fix
		{
			mt::TVectorPoints v; //FIXME: !!
			float angle;
			float s;
			float t;
		};

		void updateTileBoundingBox(mt::box2f& _boundingBox, const std::vector<Quad>& quads);

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
		const RenderImageInterface* m_edge_image;

		void proccessEdges_();
		const TileDecl* getNextTileDecl_( const TTileDecls& _decls, std::size_t _i );

		// transformed
		TQuadMap m_tr_edges;
		TQuadMap m_tr_edge_juncs;
		TVectorQuad m_tr_juncs;
		mt::TVectorPoints m_tr_triangles;

		void updatePoints_();
		void prepareTransformed_();
		void prepareLayerEdges_();

		typedef std::vector< std::pair< const RenderImageInterface*, const RenderImageInterface* > > TVectorEdgeImages;
		TVectorEdgeImages m_edge_images;
		String m_edge_layer;

		class TilePolygonEdges
			: public Node
		{
		public:
			TilePolygonEdges();
			virtual ~TilePolygonEdges();

			void setImages( TVectorEdgeImages* _images );
			void setQuads( TilePolygon::TQuadMap* _quads );
			void setBBox( mt::box2f& _box );

		protected:
			void _render( unsigned int _debugMask ) override;
			void _updateBoundingBox( mt::box2f & _boundingBox ) override;

		protected:
			TVectorEdgeImages* m_images;
			TilePolygon::TQuadMap* m_quads;
			mt::box2f m_box;
		};

		TilePolygonEdges* m_layer_edges;
	};
};
