#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Scene.h"
#	include "Layer2D.h"

#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "LogEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(HotSpot)
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
	: m_globalMouseEventListener( false )
	, m_globalKeyEventListener( false )
	, m_onLeaveEvent( false )
	, m_onEnterEvent( false )
	, m_debugColor(0xFFFF0000)
	, m_picked( false )
	, m_renderObjectHotspot( NULL )
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
	bool HotSpot::_pickerActive()
	{
		bool input = true;
		if( m_layer != 0 )
		{
			Scene* scene = m_layer->getScene();
			if( scene != 0 )
			{
				input = !scene->getBlockInput();
			}
		}
		bool upd = getUpdatable() && m_updatable;
		return upd && input;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onLeave()
	{
		m_picked = false;
		callEvent( EVENT_LEAVE, "(O)", this->getEmbedding() );

		if( m_renderObjectHotspot != NULL )
		{
			m_renderObjectHotspot->passes[0].color = ColourValue( 1.0f, 0.0f, 0.0f, 1.0f );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onEnter()
	{
		m_picked = true;
		callEvent( EVENT_ENTER, "(O)", this->getEmbedding() );

		if( m_renderObjectHotspot != NULL )
		{
			m_renderObjectHotspot->passes[0].color = ColourValue( 1.0f, 1.0f, 0.0f, 1.0f );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_polygon.add_point( _p );

		if( m_renderObjectHotspot != NULL )
		{
			const mt::mat3f& wm = getWorldMatrix();
			mt::vec2f trP;
			mt::mul_v2_m3( trP, _p, wm );
			m_renderObjectHotspot->vertices.push_back( TVertex() );
			TVertex& vtx = m_renderObjectHotspot->vertices.back();
			vtx.pos[0] = trP.x;
			vtx.pos[1] = trP.y;
			if( m_renderObjectHotspot->passes[0].indicies.empty() == false )
			{
				m_renderObjectHotspot->passes[0].indicies.pop_back();
			}
			uint16 indSize = m_renderObjectHotspot->passes[0].indicies.size();
			m_renderObjectHotspot->passes[0].indicies.push_back( indSize );
			m_renderObjectHotspot->passes[0].indicies.push_back( 0 );
		}


		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear_points();
		if( m_renderObjectHotspot != NULL )
		{
			m_renderObjectHotspot->vertices.clear();
			m_renderObjectHotspot->passes[0].indicies.clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( HotSpot * _hotspot )
	{
		if( _hotspot == this )
		{
			return false;
		}

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		const mt::box2f & myBB = this->getBoundingBox();
		const mt::box2f & otherBB = _hotspot->getBoundingBox();

		Layer2D * layer = this->getLayer();

		if( layer == 0 )
		{
			return false;
		}
		if( layer->testBoundingBox( viewport, myBB, otherBB ) == false )
		{
			return false;
		}

		if( layer->testHotspot( viewport, this, _hotspot ) == false )
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
			XML_CASE_NODE( "Point" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &HotSpot::addPoint );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_KEY, "(OIIb)", this->getEmbedding(), _key, _char, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON, "(OIb)", this->getEmbedding(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_MOVE, "(Offi)", this->getEmbedding(), _x, _y, _whell );
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
		m_globalMouseEventListener = _value;

		if( _value )
		{
			Holder<Player>::hostage()
				->regGlobalMouseEventable( this );
		}
		else
		{
			Holder<Player>::hostage()
				->unregGlobalMouseEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::enableGlobalKeyEvent( bool _value )
	{
		m_globalKeyEventListener = _value;

		if( _value )
		{
			Holder<Player>::hostage()
				->regGlobalKeyEventable( this );
		}
		else
		{
			Holder<Player>::hostage()
				->unregGlobalKeyEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setListener()
	{
		Node::_setListener();

		registerEvent( EVENT_KEY, ("onHandleKeyEvent"), m_listener );
		registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), m_listener );
		registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), m_listener );

		registerEvent( EVENT_GLOBAL_MOUSE_BUTTON, ("onGlobalHandleMouseButtonEvent"), m_listener );
		registerEvent( EVENT_GLOBAL_MOUSE_MOVE, ("onGlobalHandleMouseMove"), m_listener );
		registerEvent( EVENT_GLOBAL_KEY, ("onGlobalHandleKeyEvent"), m_listener );
		
		m_onLeaveEvent = registerEvent( EVENT_LEAVE, ("onLeave"), m_listener );
		m_onEnterEvent = registerEvent( EVENT_ENTER, ("onEnter"), m_listener );	
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		bool res = Node::_activate();
		if( res == true )
		{
			m_picked = false;
			Holder<MousePickerSystem>::hostage()
				->regTrap( this );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		m_picked = false;
		Holder<MousePickerSystem>::hostage()
			->unregTrap( this );
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
			if( m_layer->testHotspot( viewport, this, _p ) == false )
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
			mt::reset( _boundingBox, 0.f,0.f );
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
	void HotSpot::_render( unsigned int _debugMask )
	{
#	ifndef MENGE_MASTER_RELEASE
		Node::_render( _debugMask );
		if( _debugMask & MENGE_DEBUG_HOTSPOTS )
		{
			const mt::mat3f& worldMat = getWorldMatrix();
			for( std::size_t
				it = 0,
				it_end = m_polygon.num_points();
			it != it_end; 
			++it )
			{
				mt::vec2f trP;
				mt::mul_v2_m3( trP, m_polygon[it], worldMat );
				TVertex& vtx = m_renderObjectHotspot->vertices[it];
				vtx.pos[0] = trP.x;
				vtx.pos[1] = trP.y;
			}

			Holder<RenderEngine>::hostage()
				->renderObject( m_renderObjectHotspot );
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::isPicked() const
	{
		return m_picked;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_release()
	{
		Holder<Player>::hostage()
			->unregGlobalMouseEventable( this );
		Holder<Player>::hostage()
			->unregGlobalKeyEventable( this );

		Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_renderObjectHotspot );
		m_renderObjectHotspot = NULL;

		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_renderObjectHotspot = Holder<RenderEngine>::hostage()
									->createRenderObject();
		m_renderObjectHotspot->passes.resize( 1 );
		m_renderObjectHotspot->passes[0].primitiveType = PT_LINESTRIP;
		m_renderObjectHotspot->passes[0].color = ColourValue( 1.0f, 0.0f, 0.0f, 1.0f );
		m_renderObjectHotspot->passes[0].textureStages = 1;
		m_renderObjectHotspot->passes[0].textureStage[0].image = Holder<ResourceManager>::hostage()
																->getResourceT<ResourceImage>( "WhitePixel" );
		const mt::mat3f& worldMat = getWorldMatrix();
		for( std::size_t
			it = 0,
			it_end = m_polygon.num_points();
		it != it_end; 
		++it )
		{
			mt::vec2f trP;
			mt::mul_v2_m3( trP, m_polygon[it], worldMat );
			m_renderObjectHotspot->vertices.push_back( TVertex() );
			TVertex& vtx = m_renderObjectHotspot->vertices.back();
			vtx.pos[0] = trP.x;
			vtx.pos[1] = trP.y;
			m_renderObjectHotspot->passes[0].indicies.push_back( it );
		}
		m_renderObjectHotspot->passes[0].indicies.push_back( 0 );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::mat3f& _transform, const mt::polygon& _screenPoly, const mt::mat3f& _screenTransform )
	{
		return mt::intersect_poly_poly( m_polygon, _screenPoly, _transform, _screenTransform );
	}
	//////////////////////////////////////////////////////////////////////////
}
