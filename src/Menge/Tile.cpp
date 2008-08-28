#	include "Tile.h"
#	include "TilePolygon.h"

#	include "RenderEngine.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"
#	include "ResourceImage.h"
#	include "math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Tile::Tile(float _minAngle, float _maxAngle, const String & _imagename)
		: m_minAngle(_minAngle)
		, m_maxAngle(_maxAngle)
		, m_imagename(_imagename)
		, m_invalidateRenderTile(false)
		, m_rigidBodySensor(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Tile::~Tile()
	{
		delete m_rigidBodySensor;// ?
	}
	//////////////////////////////////////////////////////////////////////////
	void Tile::invalidateTile()
	{
		m_invalidateRenderTile = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Tile::create(const mt::TVectorPoints & _poly, const mt::vec2f & _juncSize)
	{
		ResourceImage * m_imageResource =
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_imagename );

		m_image = m_imageResource->getImage(0);

		float _width = m_image->getWidth();
		float _height = m_image->getHeight();

		if(mt::angle_in_interval_deg(m_minAngle,0,0))
		{
			m_rigidBodySensor = SceneManager::createNodeT<RigidBody2D>(MENGE_TEXT("RigidBody2D"));
			m_rigidBodySensor->compile();
		}	

		mt::vec2f edge;
		mt::vec2f penumbra;

		bool found_junction = false;

		mt::vec2f track_edge;

		mt::TVectorPoints::size_type pointsNum = _poly.size();

		Quad quad;
		quad.v.resize(4);

		int track_point = 0;

		for(mt::TVectorPoints::size_type i = 0; i < pointsNum; i++)
		{
			int next_i = (i + 1) % pointsNum;

			edge = _poly[next_i] - _poly[i];

			float angle = mt::signed_angle(edge) * mt::m_rad2deg;

			if(mt::angle_in_interval_deg(angle,m_minAngle,m_maxAngle))
			{
				track_point = next_i;
				track_edge = mt::perp(edge);

				penumbra = track_edge / edge.length() * _height * 0.5f;

				quad.v[0] = _poly[i] - penumbra;
				quad.v[1] = _poly[next_i] - penumbra;
				quad.v[2] = _poly[next_i] + penumbra;
				quad.v[3] = _poly[i] + penumbra;

				quad.s = edge.length() / _width;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _height;

				m_geometry.push_back(quad);

				if(m_rigidBodySensor)
				{
					m_rigidBodySensor->_addShapeConvex(quad.v,true);
				}

				found_junction = true;

				if(i < pointsNum-1)
					continue;
			}
			
			if(found_junction)
			{
				mt::vec2f normal = (track_edge + mt::perp(edge));
				normal = normal / normal.length();
				
				penumbra = normal * _juncSize.y * 0.5f;

				mt::vec2f lpoint = _poly[track_point] - mt::perp(normal) * _juncSize.x * 0.5f;
				mt::vec2f rpoint = _poly[track_point] + mt::perp(normal) * _juncSize.x * 0.5f;

				quad.v[0] = lpoint - penumbra;
				quad.v[1] = rpoint - penumbra;
				quad.v[2] = rpoint + penumbra;
				quad.v[3] = lpoint + penumbra;

				quad.s = mt::length_v2_v2(quad.v[0], quad.v[1]) / _juncSize.x;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _juncSize.y;

				m_junks.push_back(quad);

				found_junction = false;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Tile::release()
	{
		if(m_rigidBodySensor)
		{
			m_rigidBodySensor->release();
			m_rigidBodySensor = NULL;
		}

		if(m_imageResource)
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_imageResource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Tile::updateBoundingBox(mt::box2f & _boundingBox)
	{
		for( std::vector<Quad>::size_type j = 0; j < m_geometry.size(); j++ )
		{
			mt::add_internal_point(_boundingBox, m_geometry[j].v[0]);
			mt::add_internal_point(_boundingBox, m_geometry[j].v[1]);
			mt::add_internal_point(_boundingBox, m_geometry[j].v[2]);
			mt::add_internal_point(_boundingBox, m_geometry[j].v[3]);
		}

		for( std::vector<mt::vec2f>::size_type j = 0; j < m_junks.size(); j++ )
		{
			mt::add_internal_point(_boundingBox, m_junks[j].v[0]);
			mt::add_internal_point(_boundingBox, m_junks[j].v[1]);
			mt::add_internal_point(_boundingBox, m_junks[j].v[2]);
			mt::add_internal_point(_boundingBox, m_junks[j].v[3]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Tile::renderTile(const mt::mat3f & _worldMatrix, const RenderImageInterface * _imageJunc)
	{
		// незачем. FIX me: подготовить VB с трансформированными вершинами.
		if( m_invalidateRenderTile == false )
		{
			for( std::vector<Quad>::size_type j = 0; j < m_geometry.size(); j++ )
			{
				const mt::vec2f & a = m_geometry[j].v[0];
				const mt::vec2f & b = m_geometry[j].v[1];
				const mt::vec2f & c = m_geometry[j].v[2];
				const mt::vec2f & d = m_geometry[j].v[3];

				float sx = m_geometry[j].s;
				float sy = m_geometry[j].t;

				mt::vec2f uva( 0.0f, 0.0f );
				mt::vec2f uvb( sx, 0.0f );
				mt::vec2f uvc( sx, sy );
				mt::vec2f uvd( 0.0f, sy );

				Holder<RenderEngine>::hostage()->renderTriple(_worldMatrix, a, b, d,
					uva, uvb, uvd,
					0xFFFFFFFF, m_image );

				Holder<RenderEngine>::hostage()->renderTriple(_worldMatrix, b, c, d,
					uvb, uvc, uvd,
					0xFFFFFFFF, m_image );
			}

			for( std::vector<mt::vec2f>::size_type j = 0; j < m_junks.size(); j++ )
			{
				const mt::vec2f & a = m_junks[j].v[0];
				const mt::vec2f & b = m_junks[j].v[1];
				const mt::vec2f & c = m_junks[j].v[2];
				const mt::vec2f & d = m_junks[j].v[3];

				float sx = m_junks[j].s;
				float sy = m_junks[j].t;

				mt::vec2f uva( 0.0f, 0.0f );
				mt::vec2f uvb( sx, 0.0f );
				mt::vec2f uvc( sx, sy );
				mt::vec2f uvd( 0.0f, sy );

				Holder<RenderEngine>::hostage()->renderTriple(_worldMatrix, a, b, d,
					uva, uvb, uvd,
					0xFFFFFFFF, _imageJunc );

				Holder<RenderEngine>::hostage()->renderTriple(_worldMatrix, b, c, d,
					uvb, uvc, uvd,
					0xFFFFFFFF, _imageJunc );
			}

			return;
		}

		m_invalidateRenderTile = false;
	}
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGE_MASTER_RELEASE
	void Tile::renderDebug(const mt::mat3f & _worldMatrix, unsigned long _color)
	{
		for( std::vector<Quad>::size_type j = 0; j < m_geometry.size(); j++ )
		{
			mt::vec2f pts[4];

			for( int i = 0; i < 4; i++ )
			{
				mt::mul_v2_m3( pts[i], m_geometry[j].v[i], _worldMatrix );
			}

			Holder<RenderEngine>::hostage()->renderLine(_color, pts[0], pts[1] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[0], pts[3] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[1], pts[2] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[3], pts[2] );
		}

		for( std::vector<mt::vec2f>::size_type j = 0; j < m_junks.size(); j++ )
		{
			mt::vec2f pts[4];

			for( int i = 0; i < 4; i++ )
			{
				mt::mul_v2_m3( pts[i], m_junks[j].v[i], _worldMatrix );
			}

			Holder<RenderEngine>::hostage()->renderLine(_color, pts[0], pts[1] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[0], pts[3] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[1], pts[2] );
			Holder<RenderEngine>::hostage()->renderLine(_color, pts[3], pts[2] );
		}

	}
#endif
	//////////////////////////////////////////////////////////////////////////
}