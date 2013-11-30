#	include "Arrow.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include "HotSpot.h"

#	include "Game.h"
#	include "Player.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
		: m_arrowType(EAT_POINT)
		, m_pointClick(0.f, 0.f)		
		, m_radius(0.f)
		, m_hided(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Arrow::getScene()
	{
		Scene * scene = PLAYER_SERVICE(m_serviceProvider)
			->getCurrentScene();

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	EArrowType Arrow::getArrowType() const
	{
		return m_arrowType;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setPointClick( const mt::vec2f & _offsetClick )
	{
		m_arrowType = EAT_POINT;

		m_pointClick = _offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getPointClick() const
	{
		return m_pointClick;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		if( Entity::_activate() == false )
		{
			return false;
		}

		INPUT_SERVICE(m_serviceProvider)
			->addMousePositionProvider(this);

		const mt::vec2f & cursor_pos = INPUT_SERVICE(m_serviceProvider)
			->getCursorPosition();

		mt::vec3f pos;
		pos.x = cursor_pos.x;
		pos.y = cursor_pos.y;
		pos.z = 0.f;

		this->setLocalPosition( pos );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_deactivate()
	{	
		INPUT_SERVICE(m_serviceProvider)
			->removeMousePositionProvider(this);

		Entity::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMousePositionChange( const mt::vec2f & _position )
	{
		mt::vec3f v3(_position.x, _position.y, 0.f);

		this->setLocalPosition( v3 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setPolygon( const Polygon & _polygon )
	{
		m_arrowType = EAT_POLYGON;

		m_polygon = _polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & Arrow::getPolygon() const
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setRadius( float _radius )
	{
		m_arrowType = EAT_RADIUS;

		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float Arrow::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseLeave()
	{
		Node::hide( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseEnter()
	{
		if( !m_hided )
		{
			Node::hide( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::hide( bool _value )
	{
		Node::hide( _value );
		m_hided = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) == 0 )
		{
			return;
		}

		EArrowType arrowType = this->getArrowType();

		switch( arrowType )
		{
		case EAT_POINT:
			{
				return;
			}break;
		case EAT_RADIUS:
			{
				size_t numpoints = 4;
				size_t vertexCount = numpoints * 2;

				RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
					->getDebugRenderVertex2D( vertexCount );

				if( vertices == nullptr )
				{
					return;
				}

				const mt::mat4f & worldMat = this->getWorldMatrix();

				mt::vec2f ring[4];

				float radius = this->getRadius();
				float half_radius = radius * 0.5f;
				ring[0] = mt::vec2f(0, -half_radius); 
				ring[1] = mt::vec2f(half_radius, 0);
				ring[2] = mt::vec2f(0, half_radius);
				ring[3] = mt::vec2f(-half_radius, 0);

				for( size_t i = 0; i != numpoints; ++i )
				{
					size_t j = (i + 1) % numpoints;

					mt::vec2f trP0;
					mt::mul_v2_m4( trP0, ring[i], worldMat );

					RenderVertex2D & v0 = vertices[i*2+0];

					v0.pos.x = trP0.x;
					v0.pos.y = trP0.y;
					v0.pos.z = 0.f;

					v0.color = 0x8080FFFF;
					v0.uv.x = 0.f;
					v0.uv.y = 0.f;
					v0.uv2.x = 0.f;
					v0.uv2.y = 0.f;

					mt::vec2f trP1;
					mt::mul_v2_m4( trP1, ring[j], worldMat );

					RenderVertex2D & v1 = vertices[i*2+1];

					v1.pos.x = trP1.x;
					v1.pos.y = trP1.y;
					v1.pos.z = 0.f;

					v1.color = 0x8080FFFF;
					v1.uv.x = 0.f;
					v1.uv.y = 0.f;
					v1.uv2.x = 0.f;
					v1.uv2.y = 0.f;
				}

				const RenderMaterial * debugMaterial = RENDER_SERVICE(m_serviceProvider)
					->getDebugMaterial();

				RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial, nullptr, 0
					, vertices
					, vertexCount
					);
			}break;
		case EAT_POLYGON:
			{
				size_t numpoints = boost::geometry::num_points( m_polygon );

				if( numpoints == 0 )
				{
					return;
				}

				size_t vertexCount = numpoints * 2;

				RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
					->getDebugRenderVertex2D( vertexCount );

				if( vertices == nullptr )
				{
					return;
				}

				const mt::mat4f & worldMat = this->getWorldMatrix();

				const Polygon::ring_type & ring = m_polygon.outer();

				for( size_t i = 0; i != numpoints; ++i )
				{
					size_t j = (i + 1) % numpoints;

					mt::vec2f trP0;
					mt::mul_v2_m4( trP0, ring[i], worldMat );

					RenderVertex2D & v0 = vertices[i*2+0];

					v0.pos.x = trP0.x;
					v0.pos.y = trP0.y;
					v0.pos.z = 0.f;

					v0.color = 0x8080FFFF;
					v0.uv.x = 0.f;
					v0.uv.y = 0.f;
					v0.uv2.x = 0.f;
					v0.uv2.y = 0.f;

					mt::vec2f trP1;
					mt::mul_v2_m4( trP1, ring[j], worldMat );

					RenderVertex2D & v1 = vertices[i*2+1];

					v1.pos.x = trP1.x;
					v1.pos.y = trP1.y;
					v1.pos.z = 0.f;

					v1.color = 0x8080FFFF;
					v1.uv.x = 0.f;
					v1.uv.y = 0.f;
					v1.uv2.x = 0.f;
					v1.uv2.y = 0.f;
				}

				const RenderMaterial * debugMaterial = RENDER_SERVICE(m_serviceProvider)
					->getDebugMaterial();

				RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial, nullptr, 0
					, vertices
					, vertexCount
					);
			}break;
		}
	}
}
