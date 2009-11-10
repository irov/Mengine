#	include "TilePolygon.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

#	include "math/triangulation.h"

#	include "LogEngine.h"

#	include "math/convexpoly2.h"

#	include "math/angle.h"
#	include "Scene.h"
#	include "Layer2D.h"

//	resource section
#	include "ResourceManager.h"
#	include "ResourceImage.h"
#	include "ResourceTilePolygon.h"

#	include "SceneManager.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(TilePolygon);
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygon()
	: m_tileResource("")
	, m_tilePolygonResource(0)
	, m_layer_edges( NULL )
	//, m_renderObjectPoly( NULL )
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

			XML_CASE_NODE( "Edge" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Layer", m_edge_layer );
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
		if( m_layer_edges != NULL )
		{
			m_layer_edges->activate();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_deactivate()
	{		
		if( m_layer_edges != NULL )
		{
			m_layer_edges->deactivate();
		}

		RigidBody2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_compile()
	{
		/*m_tilePolygonResource = 
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

		ResourceImage* image = m_tilePolygonResource->getImage();
		m_junc_image = m_tilePolygonResource->getPlugImage();

		float inv_width = 1.f / image->getSize( 0 ).x;
		float inv_height = 1.f / image->getSize( 0 ).y;

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

		const TTileDecls& tileDecls = m_tilePolygonResource->getTileDecls();
		for( TTileDecls::size_type i = 0; i < tileDecls.size(); i++ )
		{
			m_edge_images.push_back( std::make_pair( tileDecls[i].image, tileDecls[i].image_back ) );
			if( tileDecls[i].image != NULL )
			{
				m_edges.insert( std::make_pair( tileDecls[i].image, TVectorQuad() ) );
				RenderObject* ro = Holder<RenderEngine>::hostage()
										->createRenderObject();
				ro->material.primitiveType = PT_TRIANGLELIST;
				ro->material.textureStages = 1;
				ro->material.textureStage[0].texture = tileDecls[i].image->getImage( 0 );
				ro->material.textureStage[0].addressU = TAM_WRAP;
				ro->material.textureStage[0].addressV = TAM_WRAP;
				m_edgesRO.push_back( ro );
			}
			if( tileDecls[i].junc_image != NULL )
			{
				m_edge_juncs.insert( std::make_pair( tileDecls[i].junc_image, TVectorQuad() ) );
				RenderObject* ro = Holder<RenderEngine>::hostage()
					->createRenderObject();
				ro->material.primitiveType = PT_TRIANGLELIST;
				ro->material.textureStages = 1;
				ro->material.textureStage[0].texture = tileDecls[i].junc_image->getImage( 0 );
				ro->material.textureStage[0].addressU = TAM_WRAP;
				ro->material.textureStage[0].addressV = TAM_WRAP;
				m_edge_juncsRO.push_back( ro );
			}
		}

		m_renderObjectPoly = Holder<RenderEngine>::hostage()
								->createRenderObject();

		m_renderObjectPoly->material.primitiveType = PT_TRIANGLELIST;
		m_renderObjectPoly->material.textureStages = 1;
		m_renderObjectPoly->material.textureStage[0].texture = image->getImage( 0 );
		m_renderObjectPoly->material.textureStage[0].addressU = TAM_WRAP;
		m_renderObjectPoly->material.textureStage[0].addressV = TAM_WRAP;
		m_renderObjectPoly->vertices.resize( m_triangles.size() );
		m_renderObjectPoly->material.indicies.resize( m_triangles.size() );
		for( std::size_t i = 0; i < m_triangles.size(); i += 3 )
		{
			m_renderObjectPoly->material.indicies[i] = i;
			m_renderObjectPoly->material.indicies[i+1] = i+2;
			m_renderObjectPoly->material.indicies[i+2] = i+1;
		}

		m_juncsRO = Holder<RenderEngine>::hostage()
						->createRenderObject();

		proccessEdges_();
		prepareLayerEdges_();
		prepareTransformed_();

		mt::box2f bbox;
		_updateBoundingBox( bbox );*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		/*Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_renderObjectPoly );

		Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_juncsRO );

		for( std::vector<RenderObject*>::iterator it = m_edgesRO.begin(), it_end = m_edgesRO.end();
			it != it_end;
			++it )
		{
			Holder<RenderEngine>::hostage()
				->releaseRenderObject( (*it) );
		}

		for( std::vector<RenderObject*>::iterator it = m_edge_juncsRO.begin(), it_end = m_edge_juncsRO.end();
			it != it_end;
			++it )
		{
			Holder<RenderEngine>::hostage()
				->releaseRenderObject( (*it) );
		}


		Holder<ResourceManager>::hostage()
			->releaseResource( m_tilePolygonResource );

		m_uvs.clear();
		m_triangles.clear();
		m_juncs.clear();
		m_edge_juncs.clear();
		m_edges.clear();
		m_edge_images.clear();

		RigidBody2D::_release();*/
		/*if( m_layer_edges != NULL )
		{
			delete m_layer_edges;
			m_layer_edges = NULL;
		}*/
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
			mt::add_internal_point( _boundingBox, m_tr_triangles[i] );
		}

		/*for(size_t i = 0; i < m_tileGeometry.size(); i++)
		{
			updateTileBoundingBox(_boundingBox, *m_tileGeometry[i]);
		}

		for(size_t i = 0; i < m_junkGeometry.size(); i++)
		{
			updateTileBoundingBox(_boundingBox, *m_junkGeometry[i]);
		}*/
		if( m_layer_edges != NULL )
		{
			m_layer_edges->setBBox( _boundingBox );
			m_layer_edges->invalidateBoundingBox();
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
	void TilePolygon::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		/*if( m_invalidateColor == true )
		{
			uint32 argb = getWorldColor().getAsARGB();
			RenderObject::ApplyColor applyColor( argb );
			std::for_each( m_renderObjectPoly->vertices.begin(), m_renderObjectPoly->vertices.end(),
				applyColor );

			//m_renderObjectPoly->material.color = getWorldColor();
		}
		// render poly
		Holder<RenderEngine>::hostage()->renderObject( m_renderObjectPoly );

		// render edges
		for( std::vector<RenderObject*>::iterator it = m_edgesRO.begin(), it_end = m_edgesRO.end();
			it != it_end;
			++it )
		{
			Holder<RenderEngine>::hostage()
				->renderObject( (*it) );
		}

		// render juncs between same edges
		for( TQuadMap::iterator it = m_tr_edge_juncs.begin(), it_end = m_tr_edge_juncs.end();
			it != it_end;
			it++ )
		{
			for( TVectorQuad::iterator qit = it->second.begin(), qit_end = it->second.end();
				qit != qit_end;
				qit++ )
			{
				//Holder<RenderEngine>::hostage()
				//	->renderImage( &((*qit).a), mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), argb, it->first );
			}
		}

		// render juncs between different edges
		if( m_junc_image != NULL )
		{
			for( TVectorQuad::iterator it = m_tr_juncs.begin(), it_end = m_tr_juncs.end();
				it != it_end;
				it++ )
			{
				//Holder<RenderEngine>::hostage()
				//	->renderImage( &((*it).a), mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), argb, m_junc_image );
			}
		}*/

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
		/*mt::TVectorPoints::size_type pointsNum = m_poly.size();
		const TTileDecls& tileDecls = m_tilePolygonResource->getTileDecls();

		for( mt::TVectorPoints::size_type i = 0; i < pointsNum; i++ )
		{
			mt::TVectorPoints::size_type next_i = (i + 1) % pointsNum;
			mt::TVectorPoints::size_type next_next_i = (i + 2) % pointsNum;

			mt::vec2f edge = m_poly[next_i] - m_poly[i];
			mt::vec2f next_edge = m_poly[next_next_i] - m_poly[next_i];
			float angle = mt::signed_angle(edge) * mt::m_rad2deg;

			int wtfi = 0;
			for( TTileDecls::const_iterator 
				it = tileDecls.begin(),
				it_end = tileDecls.end();
			it != it_end;
			++it, ++wtfi )
			{
				if( mt::angle_in_interval_deg( angle, it->min_angle, it->max_angle ) )
				{
					if( it->image_resource.empty() == false )
					{
						TQuad quad;
						mt::vec2f size = it->image->getSize( 0 );
						mt::vec2f half_height = mt::perp( edge ) / edge.length() * size.y * 0.5f; 
						quad.a = m_poly[i] - half_height;
						quad.b = m_poly[next_i] - half_height;
						quad.c = m_poly[next_i] + half_height;
						quad.d = m_poly[i] + half_height;

						quad.s = edge.length() / size.x;
						quad.t = mt::length_v2_v2(quad.b, quad.c) / size.y;

						m_edges[ it->image ].push_back( quad );

						m_edgesRO[wtfi]->vertices.push_back( TVertex() );
						TVertex& vtx1 = m_edgesRO[wtfi]->vertices.back();
						vtx1.pos[0] = quad.a.x;
						vtx1.pos[1] = quad.a.y;
						vtx1.uv[0] = 0.0f;
						vtx1.uv[1] = 0.0f;

						m_edgesRO[wtfi]->vertices.push_back( TVertex() );
						TVertex& vtx2 = m_edgesRO[wtfi]->vertices.back();
						vtx2.pos[0] = quad.b.x;
						vtx2.pos[1] = quad.b.y;
						vtx2.uv[0] = quad.s;
						vtx2.uv[1] = 0.0f;

						m_edgesRO[wtfi]->vertices.push_back( TVertex() );
						TVertex& vtx3 = m_edgesRO[wtfi]->vertices.back();
						vtx3.pos[0] = quad.c.x;
						vtx3.pos[1] = quad.c.y;
						vtx3.uv[0] = quad.s;
						vtx3.uv[1] = quad.t;

						m_edgesRO[wtfi]->vertices.push_back( TVertex() );
						TVertex& vtx4 = m_edgesRO[wtfi]->vertices.back();
						vtx4.pos[0] = quad.d.x;
						vtx4.pos[1] = quad.d.y;
						vtx4.uv[0] = 0.0f;
						vtx4.uv[1] = quad.t;

						uint16 idx = m_edgesRO[wtfi]->material.indicies.size();
						for( int idxi = 0; i < 4; ++idxi, ++idx )
						{
							m_edgesRO[wtfi]->material.indicies.push_back( idx );
						}
					}

					const TileDecl* nextDecl = getNextTileDecl_( tileDecls, next_i );

					if( nextDecl == &( *it ) && it->junc_image_resource.empty() == false )
					{
						TQuad quad;
						mt::vec2f size = it->junc_image->getSize( 0 );
						mt::vec2f normal = mt::perp( edge ) + mt::perp( next_edge );
						mt::vec2f half_height = normal / normal.length() * size.y * 0.5f; 
						mt::vec2f half_width = mt::perp( normal ) / normal.length() * size.x * 0.5f;
						quad.a = m_poly[next_i] - half_height - half_width;
						quad.b = m_poly[next_i] - half_height + half_width;
						quad.c = m_poly[next_i] + half_height + half_width;
						quad.d = m_poly[next_i] + half_height - half_width;

						m_edge_juncs[ it->junc_image ].push_back( quad );
					}
					else if( nextDecl != &( *it ) && m_junc_image != NULL )
					{
						TQuad quad;
						mt::vec2f size = m_junc_image->getSize( 0 );
						mt::vec2f normal = mt::perp( edge ) + mt::perp( next_edge );
						mt::vec2f half_height = normal / normal.length() * size.y * 0.5f; 
						mt::vec2f half_width = mt::perp( normal ) / normal.length() * size.x * 0.5f;
						quad.a = m_poly[next_i] - half_height - half_width;
						quad.b = m_poly[next_i] - half_height + half_width;
						quad.c = m_poly[next_i] + half_height + half_width;
						quad.d = m_poly[next_i] + half_height - half_width;

						m_juncs.push_back( quad );
					}
					
				}
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	const TileDecl* TilePolygon::getNextTileDecl_( const TTileDecls& _decls, std::size_t _i )
	{
		std::size_t next_i = ( _i + 1 ) % m_poly.size();
		mt::vec2f edge = m_poly[next_i] - m_poly[_i];
		float angle = mt::signed_angle(edge) * mt::m_rad2deg;
		for( TTileDecls::const_iterator it = _decls.begin(), it_end = _decls.end();
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
	void TilePolygon::updatePoints_()
	{
		const mt::mat3f& wm = getWorldMatrix();
		/*for( TQuadMap::iterator it = m_edges.begin(), it_end = m_edges.end();
			it != it_end;
			it++ )
		{
			TQuadMap::iterator find_it = m_tr_edges.find( it->first );
			assert( find_it !=  m_tr_edges.end() );
			for( TVectorQuad::iterator qit = it->second.begin(), qit_end = it->second.end(),
				tr_qit = find_it->second.begin();
				qit != qit_end;
				qit++, tr_qit++ )
			{
				TQuad q;
				mt::mul_v2_m3( q.a, (*qit).a, wm );
				mt::mul_v2_m3( q.b, (*qit).b, wm );
				mt::mul_v2_m3( q.c, (*qit).c, wm );
				mt::mul_v2_m3( q.d, (*qit).d, wm );
				q.s = (*qit).s;
				q.t = (*qit).t;
				(*tr_qit) = q;
			}
		}*/
		/*for( std::vector<RenderObject*>::iterator it = m_edgesRO.begin(), it_end = m_edgesRO.end();
			it != it_end;
			++it )
		{
			for( std::vector<TVertex>::iterator vit = (*it)->vertices.begin(), vit_end = (*it)->vertices.end();
				vit != vit_end;
				++vit )
			{
				TVertex& vtx = (*vit);
				mt::vec2f vec( vtx.pos[0], vtx.pos[1] );
				mt::vec2f trvec;
				mt::mul_v2_m3( trvec, vec, wm );
				vtx.pos[0] = trvec.x;
				vtx.pos[1] = trvec.y;
			}
		}

		for( TQuadMap::iterator it = m_edge_juncs.begin(), it_end = m_edge_juncs.end();
			it != it_end;
			it++ )
		{
			TQuadMap::iterator find_it = m_tr_edge_juncs.find( it->first );
			assert( find_it !=  m_tr_edges.end() );
			for( TVectorQuad::iterator qit = it->second.begin(), qit_end = it->second.end(),
				tr_qit = find_it->second.begin();
				qit != qit_end;
				qit++, tr_qit++ )
			{
				TQuad q;
				mt::mul_v2_m3( q.a, (*qit).a, wm );
				mt::mul_v2_m3( q.b, (*qit).b, wm );
				mt::mul_v2_m3( q.c, (*qit).c, wm );
				mt::mul_v2_m3( q.d, (*qit).d, wm );
				q.s = (*qit).s;
				q.t = (*qit).t;
				(*tr_qit) = q;
			}
		}

		for( TVectorQuad::iterator qit = m_juncs.begin(), qit_end = m_juncs.end(),
			tr_qit = m_tr_juncs.begin();
			qit != qit_end;
			qit++, tr_qit++ )
		{
			TQuad q;
			mt::mul_v2_m3( q.a, (*qit).a, wm );
			mt::mul_v2_m3( q.b, (*qit).b, wm );
			mt::mul_v2_m3( q.c, (*qit).c, wm );
			mt::mul_v2_m3( q.d, (*qit).d, wm );
			q.s = (*qit).s;
			q.t = (*qit).t;
			(*tr_qit) = q;
		}

		std::size_t vertCount = 0;
		for( mt::TVectorPoints::iterator it = m_triangles.begin(), it_end = m_triangles.end(),
			tr_it = m_tr_triangles.begin();
			it != it_end;
			it++, tr_it++, vertCount++ )
		{
			mt::vec2f v;
			mt::mul_v2_m3( v, (*it), wm );
			m_renderObjectPoly->vertices[vertCount].pos[0] = v.x;
			m_renderObjectPoly->vertices[vertCount].pos[1] = v.y;
			m_renderObjectPoly->vertices[vertCount].uv[0] = m_uvs[vertCount].x;
			m_renderObjectPoly->vertices[vertCount].uv[1] = m_uvs[vertCount].y;
			(*tr_it) = v;
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		invalidateBoundingBox();
		updatePoints_();
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::prepareTransformed_()
	{
		/*m_tr_edges.clear();
		m_tr_edge_juncs.clear();
		m_tr_juncs.clear();
		m_tr_triangles.clear();

		for( TQuadMap::iterator it = m_edges.begin(), it_end = m_edges.end();
			it != it_end;
			it++ )
		{
			TVectorQuad quads;
			quads.resize( it->second.size() );
			m_tr_edges.insert( std::make_pair( it->first, quads ) );
		}

		for( TQuadMap::iterator it = m_edge_juncs.begin(), it_end = m_edge_juncs.end();
			it != it_end;
			it++ )
		{
			TVectorQuad quads;
			quads.resize( it->second.size() );
			m_tr_edge_juncs.insert( std::make_pair( it->first, quads ) );
		}

		m_tr_juncs.resize( m_juncs.size() );
		m_tr_triangles.resize( m_triangles.size() );

		if( m_layer_edges != NULL )
		{
			m_layer_edges->setQuads( &m_tr_edges );
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::prepareLayerEdges_()
	{
		/*if( m_edge_layer.empty() == true )
		{
			return;
		}
		Layer* layer = static_cast<Layer*>( m_layer->getScene()->getChildren( m_edge_layer, false ) );
		if( layer == NULL )
		{
			MENGE_LOG_ERROR( "TilePolygon compile error: Layer '%s' does not exist"
				, m_edge_layer.c_str() );
			return;
		}
		
		m_layer_edges = new TilePolygonEdges();

		layer->addChildren( m_layer_edges );
		m_layer_edges->setImages( &m_edge_images ); */
	}
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygonEdges::TilePolygonEdges()
		: m_images( NULL )
		, m_quads( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygonEdges::~TilePolygonEdges()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::TilePolygonEdges::setImages( TilePolygon::TVectorEdgeImages* _images )
	{
		m_images = _images;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::TilePolygonEdges::setQuads( TilePolygon::TQuadMap* _quads )
	{
		m_quads = _quads;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::TilePolygonEdges::setBBox( mt::box2f& _box )
	{
		m_box = _box;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::TilePolygonEdges::_render( Camera2D * _camera )
	{
		/*if( m_quads == NULL	|| m_images == NULL )
		{
			return;
		}

		ColourValue color = getWorldColor();
		unsigned int argb = color.getAsARGB();

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		for( TilePolygon::TVectorEdgeImages::iterator it = m_images->begin(), it_end = m_images->end();
			it != it_end;
			it++ )
		{
			TilePolygon::TQuadMap::iterator it_find = m_quads->find( it->first );
			if( it_find == m_quads->end() || it->second == NULL )
			{
				continue;
			}

			for( TilePolygon::TVectorQuad::iterator qit = it_find->second.begin(),
				qit_end = it_find->second.end();
				qit != qit_end;
			qit++ )
			{
				mt::vec2f uv0( 0.0f, 0.0f );
				mt::vec2f uv1( (*qit).s, 0.0f );
				mt::vec2f uv2( (*qit).s, (*qit).t );
				mt::vec2f uv3( 0.0f, (*qit).t );

				//renderEngine->renderTriple( (*qit).a, (*qit).b, (*qit).d,
				//	uv0, uv1, uv3, argb, it->second );
				//renderEngine->renderTriple( (*qit).b, (*qit).c, (*qit).d,
				//	uv1, uv2, uv3, argb, it->second );
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::TilePolygonEdges::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );
		mt::merge_box( _boundingBox, m_box );
	}
	//////////////////////////////////////////////////////////////////////////
}
