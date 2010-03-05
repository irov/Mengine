#	pragma once
#	include "math/vec2.h"
#	include "math/polygon.h"
#	include "RigidBody2D.h"
#	include "ResourceTilePolygon.h"

namespace Menge
{
	class ResourceImage;
	struct Material;

	class TilePolygon
		: public RigidBody2D
	{
	public:
		TilePolygon();

	public:
		void loader( XmlElement * _xml ) override;
		void _loaderVertices( XmlElement * _xml );
		void _render( Camera2D * _camera ) override;
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
		typedef std::map< ResourceImage*, TVectorQuad > TQuadMap;
		TQuadMap m_edges;		// quads on edges with texture
		TQuadMap m_edge_juncs;	// quads between edges with same texture
		TVectorQuad m_juncs;	// quads between edges with different texture
		//std::vector<RenderObject*> m_edgesRO;
		//std::vector<RenderObject*> m_edge_juncsRO;
		//RenderObject* m_renderObjectPoly;
		//RenderObject* m_juncsRO;

		ResourceTilePolygon * m_tilePolygonResource;
		String m_tileResource;

		mt::TVectorPoints m_triangles;
		mt::TVectorPoints m_uvs;
		mt::TVectorPoints m_poly;
		ResourceImage* m_junc_image;
		ResourceImage* m_edge_image;

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

		typedef std::vector< std::pair< ResourceImage*, ResourceImage* > > TVectorEdgeImages;
		TVectorEdgeImages m_edge_images;
		String m_edge_layer;

		class TilePolygonEdges
			: public Node
		{
		public:
			TilePolygonEdges();
			~TilePolygonEdges();

			void setImages( TVectorEdgeImages* _images );
			void setQuads( TilePolygon::TQuadMap* _quads );
			void setBBox( mt::box2f& _box );

		protected:
			void _render( Camera2D * _camera ) override;
			void _updateBoundingBox( mt::box2f & _boundingBox ) override;

		protected:
			TVectorEdgeImages* m_images;
			TilePolygon::TQuadMap* m_quads;
			mt::box2f m_box;
		};

		TilePolygonEdges* m_layer_edges;

	};
};
