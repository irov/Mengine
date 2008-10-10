#	include "TilePolygon.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

#	include "math/triangulation.h"

#	include "LogEngine.h"

#	include "math/convexpoly2.h"

#	include "math/angle.h"

//	resource section
#	include "ResourceManager.h"
#	include "ResourceImage.h"
#	include "ResourceTilePolygon.h"

#	include "SceneManager.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TilePolygon);
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygon()
	: m_tileResource("")
	, m_tilePolygonResource(0)
	, m_triangles(0)
	, m_uvs(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Resource" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", m_tileResource );
				}
			}

			XML_CASE_NODE( "Vertex" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &TilePolygon::addVertex );
				}
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_activate()
	{
		if( RigidBody2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_deactivate()
	{
		RigidBody2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_compile()
	{
		m_tilePolygonResource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceTilePolygon>( m_tileResource );

		mt::TVectorPoints::size_type size = m_poly.size();
		mt::TVectorPoints::size_type capacity = 5 * size;

		m_triangles.reserve(capacity);
		m_uvs.reserve(capacity);

		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		if(result == false)
		{
			MENGE_LOG_ERROR( "can't triangulate polygon" );
			return false;
		}

		m_image = m_tilePolygonResource->getImage();
		m_junc_image = m_tilePolygonResource->getPlugImage();

		float inv_width = 1.f / m_image->getWidth();
		float inv_height = 1.f / m_image->getHeight();

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles.size(); i++)
		{
			mt::vec2f uv(m_triangles[i].x * inv_width, m_triangles[i].y * inv_height);
			m_uvs.push_back(uv);
		}

		mt::decompose_concave(m_poly,m_shapeList);

		if(result == false)
		{
			MENGE_LOG_ERROR( "can't divide into polygons" );
			return false;
		}

		if(RigidBody2D::_compile() == false)
		{
			return false;
		}

		const ResourceTilePolygon::TTileDecls& tileDecls = m_tilePolygonResource->getTileDecls();
		for( ResourceTilePolygon::TTileDecls::size_type i = 0; i < tileDecls.size(); i++ )
		{
			if( tileDecls[i].image != NULL )
			{
				m_edges.insert( std::make_pair( tileDecls[i].image, TVectorQuad() ) );
			}
			if( tileDecls[i].junc_image != NULL )
			{
				m_edge_juncs.insert( std::make_pair( tileDecls[i].junc_image, TVectorQuad() ) );
			}
		}

		proccessEdges_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_tilePolygonResource );

		RigidBody2D::_release();

		m_uvs.clear();
		m_triangles.clear();
		m_juncs.clear();
		m_edge_juncs.clear();
		m_edges.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_update( float _timing )
	{
		RigidBody2D::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		if( m_triangles.empty() ) return;

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles.size(); i++)
		{
			mt::add_internal_point( _boundingBox, m_triangles[i] );
		}

		for(size_t i = 0; i < m_tileGeometry.size(); i++)
		{
			updateTileBoundingBox(_boundingBox, *m_tileGeometry[i]);
		}

		for(size_t i = 0; i < m_junkGeometry.size(); i++)
		{
			updateTileBoundingBox(_boundingBox, *m_junkGeometry[i]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::updateTileBoundingBox(mt::box2f& _boundingBox, const std::vector<Quad>& quads)
	{
		for( std::vector<Quad>::size_type j = 0; j < quads.size(); j++ )
		{
			mt::add_internal_point(_boundingBox, quads[j].v[0]);
			mt::add_internal_point(_boundingBox, quads[j].v[1]);
			mt::add_internal_point(_boundingBox, quads[j].v[2]);
			mt::add_internal_point(_boundingBox, quads[j].v[3]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );

		const mt::mat3f & wm = getWorldMatrix();

		// render poly
		for(std::vector<mt::vec2f>::size_type i = 0; i < m_triangles.size(); i+=3)
		{
			const mt::vec2f & v0 = m_triangles[i+0];
			const mt::vec2f & v1 = m_triangles[i+1];
			const mt::vec2f & v2 = m_triangles[i+2];

			const mt::vec2f & uv0 = m_uvs[i+0];
			const mt::vec2f & uv1 = m_uvs[i+1];
			const mt::vec2f & uv2 = m_uvs[i+2];

			const RenderImageInterface * image = m_tilePolygonResource->getImage();

			Holder<RenderEngine>::hostage()->renderTriple(wm, v0, v1, v2,
				uv0, uv1, uv2,
				0xFFFFFFFF, image );
		}

		// render edges
		for( TQuadMap::iterator it = m_edges.begin(), it_end = m_edges.end();
			it != it_end;
			it++ )
		{
			for( TVectorQuad::iterator qit = it->second.begin(), qit_end = it->second.end();
				qit != qit_end;
				qit++ )
			{

				mt::vec2f uv0( 0.0f, 0.0f );
				mt::vec2f uv1( (*qit).s, 0.0f );
				mt::vec2f uv2( (*qit).s, (*qit).t );
				mt::vec2f uv3( 0.0f, (*qit).t );
				//mt::vec4f uv( 0.0f, 0.0f, (*qit).s, (*qit).t );
				Holder<RenderEngine>::hostage()
					->renderTriple( wm, (*qit).a, (*qit).b, (*qit).d,
					uv0, uv1, uv3, 0xFFFFFFFF, it->first );
				Holder<RenderEngine>::hostage()
					->renderTriple( wm, (*qit).b, (*qit).c, (*qit).d,
					uv1, uv2, uv3, 0xFFFFFFFF, it->first );

			}
		}

		// render juncs between same edges
		for( TQuadMap::iterator it = m_edge_juncs.begin(), it_end = m_edge_juncs.end();
			it != it_end;
			it++ )
		{
			for( TVectorQuad::iterator qit = it->second.begin(), qit_end = it->second.end();
				qit != qit_end;
				qit++ )
			{
				Holder<RenderEngine>::hostage()
					->renderImage( &((*qit).a), mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF, it->first );
			}
		}

		// render juncs between different edges
		if( m_junc_image != NULL )
		{
			for( TVectorQuad::iterator it = m_juncs.begin(), it_end = m_juncs.end();
				it != it_end;
				it++ )
			{
				Holder<RenderEngine>::hostage()
					->renderImage( &((*it).a), mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF, m_junc_image );
			}
		}

#	ifndef MENGE_MASTER_RELEASE
		/*	if( _debugMask & MENGE_DEBUG_TILEPOLYGON )
		{
		int i = 0;
		for(std::list<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++,i++)
		{
		it->renderDebug(wm, 0xFF00FF00 +  ((i+1)*105));
		}
		}*/
# endif
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::proccessEdges_()
	{
		mt::TVectorPoints::size_type pointsNum = m_poly.size();
		const ResourceTilePolygon::TTileDecls& tileDecls = m_tilePolygonResource->getTileDecls();

		for( std::size_t i = 0; i < pointsNum; i++ )
		{
			int next_i = (i + 1) % pointsNum;
			int next_next_i = (i + 2) % pointsNum;

			mt::vec2f edge = m_poly[next_i] - m_poly[i];
			mt::vec2f next_edge = m_poly[next_next_i] - m_poly[next_i];
			float angle = mt::signed_angle(edge) * mt::m_rad2deg;
			for( ResourceTilePolygon::TTileDecls::size_type j = 0; j < tileDecls.size(); j++ )
			{
				if( mt::angle_in_interval_deg( angle, tileDecls[j].min_angle, tileDecls[j].max_angle ) )
				{
					if( tileDecls[i].image_resource.empty() == false )
					{
						TQuad quad;
						float width = tileDecls[j].image->getWidth();
						float height = tileDecls[j].image->getHeight();
						mt::vec2f half_height = mt::perp( edge ) / edge.length() * height * 0.5f; 
						quad.a = m_poly[i] - half_height;
						quad.b = m_poly[next_i] - half_height;
						quad.c = m_poly[next_i] + half_height;
						quad.d = m_poly[i] + half_height;

						quad.s = edge.length() / width;
						quad.t = mt::length_v2_v2(quad.b, quad.c) / height;

						m_edges[ tileDecls[j].image ].push_back( quad );
					}

					const ResourceTilePolygon::TileDecl* nextDecl = getNextTileDecl_( tileDecls, next_i );

					if( nextDecl == &( tileDecls[j] ) && tileDecls[j].junc_image_resource.empty() == false )
					{
						TQuad quad;
						float width = tileDecls[j].junc_image->getWidth();
						float height = tileDecls[j].junc_image->getHeight();
						mt::vec2f normal = mt::perp( edge ) + mt::perp( next_edge );
						mt::vec2f half_height = normal / normal.length() * height * 0.5f; 
						mt::vec2f half_width = mt::perp( normal ) / normal.length() * width * 0.5f;
						quad.a = m_poly[next_i] - half_height - half_width;
						quad.b = m_poly[next_i] - half_height + half_width;
						quad.c = m_poly[next_i] + half_height + half_width;
						quad.d = m_poly[next_i] + half_height - half_width;

						m_edge_juncs[ tileDecls[j].junc_image ].push_back( quad );
					}
					else if( nextDecl != &( tileDecls[j] ) && m_junc_image != NULL )
					{
						TQuad quad;
						float width = m_junc_image->getWidth();
						float height = m_junc_image->getHeight();
						mt::vec2f normal = mt::perp( edge ) + mt::perp( next_edge );
						mt::vec2f half_height = normal / normal.length() * height * 0.5f; 
						mt::vec2f half_width = mt::perp( normal ) / normal.length() * width * 0.5f;
						quad.a = m_poly[next_i] - half_height - half_width;
						quad.b = m_poly[next_i] - half_height + half_width;
						quad.c = m_poly[next_i] + half_height + half_width;
						quad.d = m_poly[next_i] + half_height - half_width;

						m_juncs.push_back( quad );
					}
					
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceTilePolygon::TileDecl* TilePolygon::getNextTileDecl_( const ResourceTilePolygon::TTileDecls& _decls, int _i )
	{
		int next_i = ( _i + 1 ) % m_poly.size();
		mt::vec2f edge = m_poly[next_i] - m_poly[_i];
		float angle = mt::signed_angle(edge) * mt::m_rad2deg;
		for( ResourceTilePolygon::TTileDecls::const_iterator it = _decls.begin(), it_end = _decls.end();
			it != it_end;
			it++ )
		{
			if( mt::angle_in_interval_deg( angle, (*it).min_angle, (*it).max_angle ) )
			{
				return &(*it);
			}
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}