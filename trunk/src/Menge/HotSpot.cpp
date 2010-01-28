#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Scene.h"
#	include "Camera2D.h"
#	include "Layer2D.h"

#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "LogEngine.h"

#	include "Application.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListener( false )
	, m_globalKeyEventListener( false )
	, m_onLeaveEvent( false )
	, m_onEnterEvent( false )
	, m_pickerId(0)
#	ifndef MENGE_MASTER_RELEASE
	, m_debugColor(0x8000FF00)
#	endif
	{
		this->setHandler( this );
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::polygon & HotSpot::getPolygon() const
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_pickerActive() const
	{
		if( m_layer != 0 )
		{
			Scene* scene = m_layer->getScene();
			if( scene != 0 )
			{
				if( scene->getBlockInput() == true )
				{
					return false;
				}
			}
		}

		if( m_enable == false )
		{
			return false;
		}

		if( m_updatable == false )
		{
			return false;
		}

		if( this->getUpdatable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onLeave()
	{
		if( m_onLeaveEvent )
		{
			callEvent( EVENT_LEAVE, "(O)", this->getEmbedding() );
		}

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor |= 0x00FF0000;
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::onEnter()
	{
		//if( m_onEnterEvent )
		//{
		//	callEvent( EVENT_ENTER, "(O)", this->getEmbedding() );
		//}

		bool handle = false;

		if( m_onEnterEvent )
		{
			if( !handle )
			{
				if( askEvent( handle, EVENT_ENTER, "(O)", this->getEmbedding() ) == false )
				{
					handle = true;
				}
			}
		}
		else
		{
			handle = true;
		}

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor &= 0xFF00FFFF;
		VectorVertices::invalidateVertices();
#	endif

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_polygon.add_point( _p );

		invalidateBoundingBox();

#	ifndef MENGE_MASTER_RELEASE
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear_points();

#	ifndef MENGE_MASTER_RELEASE
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( Arrow * _arrow )
	{
		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		//const mt::box2f & myBB = this->getBoundingBox();
		//const mt::box2f & otherBB = _hotspot->getBoundingBox();

		Layer * layer = this->getLayer();

		if( layer == 0 )
		{
			return false;
		}

		//if( layer->testBoundingBox( viewport, myBB, otherBB ) == false )
		//{
		//	return false;
		//}

		if( layer->testArrow( viewport, this, _arrow ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( XmlElement * _xml)
	{
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Point", "Value", &HotSpot::addPoint ); //depricated
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Polygon", "Point", &HotSpot::addPoint );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( askEvent( handle, EVENT_KEY, "(OIIb)", this->getEmbedding(), _key, _char, _isDown ) == false )
			{
				handle = true;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( askEvent( handle, EVENT_MOUSE_BUTTON, "(OIb)", this->getEmbedding(), _button, _isDown ) == false )
			{
				handle = true;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			if( askEvent( handle, EVENT_MOUSE_MOVE, "(Offi)", this->getEmbedding(), _x, _y, _whell ) == false )
			{
				handle = true;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_GLOBAL_MOUSE_BUTTON, "(OIb)", this->getEmbedding(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_GLOBAL_MOUSE_MOVE, "(Offi)", this->getEmbedding(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_GLOBAL_KEY, "(OIIb)", this->getEmbedding(), _key, _char, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalMouseEvent( bool _value )
	{
		if( m_globalMouseEventListener == _value )
		{
			return;
		}

		m_globalMouseEventListener = _value;

		if( this->isActivate() )
		{
			this->applyGlobalMouseEvent_( m_globalMouseEventListener );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::applyGlobalMouseEvent_( bool _value )
	{
		Scene * scene = this->getScene();

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "Error: HotSpot %s enableGlobalMouseEvent not enter scene"
				, m_name.c_str()
				);

			return;
		}

		if( _value )
		{
			scene->regGlobalMouseEventable( this );
		}
		else
		{
			scene->unregGlobalMouseEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalKeyEvent( bool _value )
	{
		if( m_globalKeyEventListener == _value )
		{
			return;
		}

		m_globalKeyEventListener = _value;

		if( this->isActivate() )
		{
			this->applyGlobalKeyEvent_( m_globalKeyEventListener );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::applyGlobalKeyEvent_( bool _value )
	{
		Scene * scene = this->getScene();

		if( scene == 0 )
		{
			MENGE_LOG_ERROR( "Error: HotSpot %s enableGlobalKeyEvent not enter scene"
				, m_name.c_str()
				);

			return;
		}

		if( _value )
		{
			scene->regGlobalKeyEventable( this );
		}
		else
		{
			scene->unregGlobalKeyEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setListener()
	{
		Node::_setListener();

		Eventable::registerEvent( EVENT_KEY, ("onHandleKeyEvent"), m_listener );
		Eventable::registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), m_listener );
		Eventable::registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), m_listener );

		Eventable::registerEvent( EVENT_GLOBAL_MOUSE_BUTTON, ("onGlobalHandleMouseButtonEvent"), m_listener );
		Eventable::registerEvent( EVENT_GLOBAL_MOUSE_MOVE, ("onGlobalHandleMouseMove"), m_listener );
		Eventable::registerEvent( EVENT_GLOBAL_KEY, ("onGlobalHandleKeyEvent"), m_listener );
		
		m_onLeaveEvent = Eventable::registerEvent( EVENT_LEAVE, ("onLeave"), m_listener );
		m_onEnterEvent = Eventable::registerEvent( EVENT_ENTER, ("onEnter"), m_listener );	
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_changeParent( Node * _parent )
	{
		Layer * layer = _parent->getLayer();

		bool activate = this->isActivate();

		if( layer && activate )
		{
			MousePickerSystem * mps = layer->getMousePickerSystem();
			mps->unregTrap( m_pickerId );
		}

		m_pickerId = 0;

		if( m_layer && activate )
		{
			MousePickerSystem * mps = m_layer->getMousePickerSystem();
			m_pickerId = mps->regTrap( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_layer )
		{
			MousePickerSystem * mps = m_layer->getMousePickerSystem();
			m_pickerId = mps->regTrap( this );
		}

		if( m_globalKeyEventListener == true )
		{
			applyGlobalKeyEvent_( true );
		}
		
		if( m_globalMouseEventListener == true )
		{
			applyGlobalMouseEvent_( true );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		if( m_globalKeyEventListener == true )
		{
			applyGlobalKeyEvent_( false );
		}

		if( m_globalMouseEventListener == true )
		{
			applyGlobalMouseEvent_( false );
		}

		if( m_layer )
		{
			MousePickerSystem * mps = m_layer->getMousePickerSystem();
			mps->unregTrap( m_pickerId );
		}
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_enable()
	{
#	ifndef MENGE_MASTER_RELEASE
		m_debugColor |= 0x000000FF;
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_disable()
	{
#	ifndef MENGE_MASTER_RELEASE
		m_debugColor &= 0xFFFFFF00;
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		//const mt::vec2f & direction = this->getWorldDirection();
		//const mt::vec2f & position = this->getScreenPosition(;
		const mt::box2f& myBBox = getBoundingBox();

		if( m_layer != 0 )
		{
			if( m_layer->testBoundingBox( viewport, myBBox, mt::box2f( _p, _p ) ) == false )
			{
				return false;
			}

			if( m_layer->testPoint( viewport, this, _p ) == false )
			{
				return false;
			}
		}
		else
		{
			const mt::mat3f& wm = getWorldMatrix();
			bool result = mt::is_point_inside_polygon( m_polygon, _p, wm );
			return result;
		}

		//bool result = mt::is_point_inside_polygon(m_polygon, _p, position, direction);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		std::size_t numPoints = m_polygon.num_points();

		if( numPoints == 0 )
		{
			Node::_updateBoundingBox( _boundingBox );
			return;
		}

		const mt::mat3f & wm = this->getWorldMatrix();

		mt::reset( _boundingBox, m_polygon[0] * wm );

		for( std::size_t
			it = 1,
			it_end = m_polygon.num_points();
		it != it_end; 
		++it )
		{
			mt::add_internal_point( _boundingBox, m_polygon[it] * wm );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_release()
	{
		if( m_globalKeyEventListener == true )
		{
			enableGlobalKeyEvent( false );
		}

		if( m_globalMouseEventListener == true )
		{
			enableGlobalMouseEvent( false );
		}

		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_layer )
		{
			MousePickerSystem * mps = m_layer->getMousePickerSystem();
			mps->updateTrap( m_pickerId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform )
	{
		return mt::intersect_poly_poly( m_polygon, _screenPoly, _transform, _screenTransform );
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( Camera2D * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) <= 0 )
		{
			return;
		}

		VectorVertices::TVectorVertex2D & vertices = getVertices();

		if( vertices.empty() )
		{
			return;
		}

		Holder<RenderEngine>::hostage()
			->renderObject2D( m_debugMaterial, NULL, 1, &(vertices[0]), vertices.size(), LPT_LINE );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate )
	{
		std::size_t numpoints = m_polygon.num_points();
		if( numpoints == 0 )
		{
			return;
		}

		_vertices.resize( numpoints + 1 );

		const mt::mat3f & worldMat = getWorldMatrix();
		for( std::size_t i = 0; i < numpoints; ++i )
		{
			mt::vec2f trP;
			mt::mul_v2_m3( trP, m_polygon[i], worldMat );
			_vertices[i].pos[0] = trP.x;
			_vertices[i].pos[1] = trP.y;
			_vertices[i].color = m_debugColor;
		}

		if( _vertices.size() > 1 )
		{
			std::copy( _vertices.begin(), _vertices.begin() + 1, _vertices.end() - 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		VectorVertices::invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
#	endif
}
