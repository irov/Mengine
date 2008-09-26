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
	, m_rigidBodySensor(0)
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
		}
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
		m_rigidBodySensor = SceneManager::createNodeT<RigidBody2D>("RigidBody2D");
		m_rigidBodySensor->compile();

		m_tilePolygonResource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceTilePolygon>( m_tileResource );

		m_triangles = m_tilePolygonResource->getTriangles();
		m_uvs = m_tilePolygonResource->getTextureCoords();

		m_shapeList = *m_tilePolygonResource->getShapeList();

		if(RigidBody2D::_compile() == false)
		{
			return false;
		}

		size_t count = m_tilePolygonResource->getTileCount();

		for(size_t i = 0; i < count; i++)
		{
			const TListQuad * tileGeoms = m_tilePolygonResource->getTileGeometry(i);
			const TListQuad * tileJunks = m_tilePolygonResource->getTileJunks(i);

			m_tileGeometry.push_back(tileGeoms);
			m_junkGeometry.push_back(tileJunks);

			if(m_tilePolygonResource->getMinAngle(i) == 0)
			{
				for( std::vector<Quad>::size_type j = 0; j < (*tileGeoms).size(); j++ )
				{
					m_rigidBodySensor->_addShapeConvex((*tileGeoms)[j].v,true);
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		if(m_rigidBodySensor)
		{
			m_rigidBodySensor->release();
			m_rigidBodySensor = NULL;
		}

		RigidBody2D::_release();
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

		if(m_triangles == NULL) return;

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles->size(); i++)
		{
			mt::add_internal_point( _boundingBox, (*m_triangles)[i] );
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

		const mt::TVectorPoints & triangles = *m_triangles;
		const mt::TVectorPoints & uvs = *m_uvs;

		for(std::vector<mt::vec2f>::size_type i = 0; i < triangles.size(); i+=3)
		{
			const mt::vec2f & v0 = triangles[i+0];
			const mt::vec2f & v1 = triangles[i+1];
			const mt::vec2f & v2 = triangles[i+2];

			const mt::vec2f & uv0 = uvs[i+0];
			const mt::vec2f & uv1 = uvs[i+1];
			const mt::vec2f & uv2 = uvs[i+2];

			const RenderImageInterface * image = m_tilePolygonResource->getImage();

			Holder<RenderEngine>::hostage()->renderTriple(wm, v0, v1, v2,
				uv0, uv1, uv2,
				0xFFFFFFFF, image );
		}

		// да да, дубликат, пофикшу, когда разберемся как стык рандомно выбирать.
		for(size_t i = 0; i < m_tileGeometry.size(); i++)
		{
			const std::vector<Quad>& quads = *m_tileGeometry[i];

			const RenderImageInterface * m_image = m_tilePolygonResource->getTileImage(i);

			for( std::vector<Quad>::size_type j = 0; j < quads.size(); j++ )
			{
				const mt::vec2f & a = quads[j].v[0];
				const mt::vec2f & b = quads[j].v[1];
				const mt::vec2f & c = quads[j].v[2];
				const mt::vec2f & d = quads[j].v[3];

				float sx = quads[j].s;
				float sy = quads[j].t;

				mt::vec2f uva( 0.0f, 0.0f );
				mt::vec2f uvb( sx, 0.0f );
				mt::vec2f uvc( sx, sy );
				mt::vec2f uvd( 0.0f, sy );

				Holder<RenderEngine>::hostage()->renderTriple(wm, a, b, d,
					uva, uvb, uvd,
					0xFFFFFFFF, m_image );

				Holder<RenderEngine>::hostage()->renderTriple(wm, b, c, d,
					uvb, uvc, uvd,
					0xFFFFFFFF, m_image );
			}
		}

		for(size_t i = 0; i < m_junkGeometry.size(); i++)
		{
			const std::vector<Quad>& quads = *m_junkGeometry[i];

			const RenderImageInterface * m_image = m_tilePolygonResource->getPlugImage();

			for( std::vector<Quad>::size_type j = 0; j < quads.size(); j++ )
			{
				const mt::vec2f & a = quads[j].v[0];
				const mt::vec2f & b = quads[j].v[1];
				const mt::vec2f & c = quads[j].v[2];
				const mt::vec2f & d = quads[j].v[3];

				float sx = quads[j].s;
				float sy = quads[j].t;

				mt::vec2f uva( 0.0f, 0.0f );
				mt::vec2f uvb( sx, 0.0f );
				mt::vec2f uvc( sx, sy );
				mt::vec2f uvd( 0.0f, sy );

				Holder<RenderEngine>::hostage()->renderTriple(wm, a, b, d,
					uva, uvb, uvd,
					0xFFFFFFFF, m_image );

				Holder<RenderEngine>::hostage()->renderTriple(wm, b, c, d,
					uvb, uvc, uvd,
					0xFFFFFFFF, m_image );
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
}