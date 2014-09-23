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
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_arrowType = EAT_POINT;

		m_pointClick = _offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getOffsetClick() const
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

		mt::vec2f wp;
		this->calcMouseWorldPosition( m_renderCamera, m_renderViewport, cursor_pos, wp );

		mt::vec3f pos;
		pos.x = wp.x;
		pos.y = wp.y;
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
	void Arrow::onMousePositionChange( const mt::vec2f & _screenPosition )
	{
		mt::vec2f wp;
		this->calcMouseWorldPosition( m_renderCamera, m_renderViewport, _screenPosition, wp );

		mt::vec3f v3(wp.x, wp.y, 0.f);

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
		if( m_hided == false )
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
	void Arrow::calcMouseWorldPosition( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const
	{
		mt::vec2f adaptScreenPoint;
		this->adaptScreenPosition_( _screenPoint, adaptScreenPoint );

		const Viewport & viewport = _viewport->getViewport();

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		mt::vec2f contentResolutionSize;
		contentResolution.calcSize( contentResolutionSize );

		mt::vec2f viewportSize;
		viewport.calcSize( viewportSize );

		mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
		mt::vec2f vp_end = viewport.end / contentResolutionSize;
		mt::vec2f vp_size = viewportSize / contentResolutionSize;

		mt::vec2f sp = (adaptScreenPoint - vp_begin) / vp_size;

		const mt::mat4f & pm_inv = _camera->getCameraProjectionMatrixInv();

		mt::vec2f p1 = sp * 2.f - mt::vec2f(1.f, 1.f);
		p1.y = -p1.y;

		mt::vec2f p_pm;
		mt::mul_v2_m4( p_pm, p1, pm_inv );

		const mt::mat4f & vm_inv = _camera->getCameraViewMatrixInv();

		mt::vec2f p = p_pm;

		mt::vec2f p_vm;
		mt::mul_v2_m4( p_vm, p, vm_inv );

		_worldPoint = p_vm;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::calcPointClick( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) const
	{
		mt::vec2f adaptScreenPoint;
		this->adaptScreenPosition_( _screenPoint, adaptScreenPoint );

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		mt::vec2f contentResolutionSize;
		contentResolution.calcSize( contentResolutionSize );

		const Viewport & viewport = _viewport->getViewport();

		mt::vec2f viewportSize;
		viewport.calcSize( viewportSize );

		mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
		mt::vec2f vp_end = viewport.end / contentResolutionSize;
		mt::vec2f vp_size = viewportSize / contentResolutionSize;

		mt::vec2f sp = (adaptScreenPoint - vp_begin) / vp_size;

		const mt::mat4f & pm_inv = _camera->getCameraProjectionMatrixInv();

		mt::vec2f p1 = sp * 2.f - mt::vec2f(1.f, 1.f);
		p1.y = -p1.y;

		mt::vec2f p_pm;
		mt::mul_v2_m4( p_pm, p1, pm_inv );

		const mt::mat4f & vm_inv = _camera->getCameraViewMatrixInv();
		
		EArrowType arrowType = this->getArrowType();

		switch( arrowType )
		{
		case EAT_POINT:
			{
				const mt::vec2f & pc = this->getOffsetClick();

				mt::vec2f p = p_pm + pc;

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, p, vm_inv );

				_worldPoint = p_vm;
			}break;
		case EAT_RADIUS:
			{
				mt::vec2f p = p_pm;

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, p, vm_inv );

				_worldPoint = p_vm;
			}break;
		case EAT_POLYGON:
			{
				mt::vec2f p = p_pm;

				mt::vec2f p_vm;
				mt::mul_v2_m4( p_vm, p, vm_inv );

				_worldPoint = p_vm;
			}break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::calcPointDeltha( const RenderCameraInterface * _camera, const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) const
	{
		mt::vec2f adaptScreenPoint;
		this->adaptScreenPosition_( _screenPoint, adaptScreenPoint );

		mt::vec2f adaptScreenDeltha;
		this->adaptScreenDeltha_( _screenDeltha, adaptScreenDeltha );
		
		mt::vec2f p1 = (adaptScreenPoint + adaptScreenDeltha) * 2.f - mt::vec2f(1.f, 1.f);
		p1.y = -p1.y;

		const mt::mat4f & pm_inv = _camera->getCameraProjectionMatrixInv();

		mt::vec2f p_pm;
		mt::mul_v2_m4( p_pm, p1, pm_inv );

		mt::vec2f p2 = (adaptScreenPoint) * 2.f - mt::vec2f(1.f, 1.f);
		p2.y = -p2.y;

		mt::vec2f p_pm_base;
		mt::mul_v2_m4( p_pm_base, p2, pm_inv );

		mt::vec2f deltha = p_pm - p_pm_base;

		_worldDeltha = deltha;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::calcMouseScreenPosition( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _worldPoint, mt::vec2f & _screenPoint ) const
	{
		const mt::mat4f & vm = _camera->getCameraViewMatrix();

		mt::vec2f p = _worldPoint;

		mt::vec2f p_vm;
		mt::mul_v2_m4( p_vm, p, vm );

		const Viewport & viewport = _viewport->getViewport();
		p_vm += viewport.begin;

		const mt::mat4f & pm = _camera->getCameraProjectionMatrix();

		mt::vec2f p_vm_pm;
		mt::mul_v2_m4( p_vm_pm, p_vm, pm );

		p_vm_pm.y = -p_vm_pm.y;

		mt::vec2f p_screen = (p_vm_pm + mt::vec2f(1.f, 1.f)) / 2.f;

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getContentResolution();

		mt::vec2f contentResolutionSize;
		contentResolution.calcSize( contentResolutionSize );

		mt::vec2f viewportSize;
		viewport.calcSize( viewportSize );

		mt::vec2f vp_begin = viewport.begin / contentResolutionSize;
		mt::vec2f vp_end = viewport.end / contentResolutionSize;
		mt::vec2f vp_size = viewportSize / contentResolutionSize;

		mt::vec2f sp = vp_begin + p_screen * vp_size;

		mt::vec2f adapt_sp;
		this->adaptWorldPosition_( sp, adapt_sp );

		_screenPoint = adapt_sp;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::adaptScreenPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const
	{
		const Viewport & renderViewport = APPLICATION_SERVICE(m_serviceProvider)
			->getRenderViewport();

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getCurrentResolution();

		mt::vec2f renderViewportSize;
		renderViewport.calcSize( renderViewportSize );

		mt::vec2f currentResolutionSize;
		currentResolution.calcSize( currentResolutionSize );

		mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
		mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

		_adaptScreenPoint = (_screenPoint - windowOffset) / windowScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::adaptScreenDeltha_( const mt::vec2f & _screenDeltha, mt::vec2f & _adaptScreenDeltha ) const
	{
		const Viewport & renderViewport = APPLICATION_SERVICE(m_serviceProvider)
			->getRenderViewport();

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getCurrentResolution();

		mt::vec2f renderViewportSize;
		renderViewport.calcSize( renderViewportSize );

		mt::vec2f currentResolutionSize;
		currentResolution.calcSize( currentResolutionSize );

		mt::vec2f windowScale = renderViewportSize / currentResolutionSize;		

		_adaptScreenDeltha = _screenDeltha / windowScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::adaptWorldPosition_( const mt::vec2f & _screenPoint, mt::vec2f & _adaptScreenPoint ) const
	{
		const Viewport & renderViewport = APPLICATION_SERVICE(m_serviceProvider)
			->getRenderViewport();

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
			->getCurrentResolution();

		mt::vec2f renderViewportSize;
		renderViewport.calcSize( renderViewportSize );

		mt::vec2f currentResolutionSize;
		currentResolution.calcSize( currentResolutionSize );

		mt::vec2f windowScale = renderViewportSize / currentResolutionSize;
		mt::vec2f windowOffset = renderViewport.begin / currentResolutionSize;

		_adaptScreenPoint = _screenPoint * windowScale + windowOffset;
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
				uint32_t numpoints = 4;
				uint32_t vertexCount = numpoints * 2;

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

				for( uint32_t i = 0; i != numpoints; ++i )
				{
					uint32_t j = (i + 1) % numpoints;

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

				const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
					->getDebugMaterial();

				RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
					, vertices
					, vertexCount
					, nullptr
					);
			}break;
		case EAT_POLYGON:
			{
				uint32_t numpoints = boost::geometry::num_points( m_polygon );

				if( numpoints == 0 )
				{
					return;
				}

				uint32_t vertexCount = numpoints * 2;

				RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
					->getDebugRenderVertex2D( vertexCount );

				if( vertices == nullptr )
				{
					return;
				}

				const mt::mat4f & worldMat = this->getWorldMatrix();

				const Polygon::ring_type & ring = m_polygon.outer();

				for( uint32_t i = 0; i != numpoints; ++i )
				{
					uint32_t j = (i + 1) % numpoints;

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

				const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
					->getDebugMaterial();

				RENDER_SERVICE(m_serviceProvider)->addRenderLine( _viewport, _camera, debugMaterial
					, vertices
					, vertexCount
					, nullptr
					);
			}break;
		}
	}
}
