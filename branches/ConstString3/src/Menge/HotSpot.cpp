#	include "HotSpot.h"

#	include "BinParser.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"

#	include "Player.h"

#	include "Camera2D.h"
#	include "Scene.h"
#	include "Layer2D.h"

#	include "ResourceManager.h"
#	include "ResourceImage.h"

#	include "Logger/Logger.h"

#	include "Application.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
		: MousePickerAdapter(true)
#	ifndef MENGE_MASTER_RELEASE
		, m_debugColor(0x80FFFFFF)
#	endif
	{
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

		if( this->isFreeze() == true )
		{
			return false;
		}

		if( this->isActivate() == false )
		{
			return false;
		}

		if( this->isEnable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onMouseLeave()
	{
		MousePickerAdapter::onMouseLeave();

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor &= 0xFF000000;
		m_debugColor |= 0x00FFFFFF;
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::onMouseEnter()
	{
		bool handle = false;

		handle = MousePickerAdapter::onMouseEnter();

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor &= 0xFF000000;
		m_debugColor |= 0x00FF0000;
		VectorVertices::invalidateVertices();
#	endif

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::addPoint( const mt::vec2f & _p )
	{
		m_polygon.add_point( _p );

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear_points();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_invalidateBoundingBox()
	{
		Node::_invalidateBoundingBox();

#	ifndef MENGE_MASTER_RELEASE
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( Arrow * _arrow )
	{
		Camera2D * camera = Player::get()
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
	bool HotSpot::isEnableGlobalHandle() const
	{
		return isActivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Point_Value, &HotSpot::addPoint ); //depricated
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Polygon_Point, &HotSpot::addPoint );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		MousePickerAdapter::setEventListener( _listener );

		GlobalHandleAdapter::setEventListener( _listener );

		Eventable::registerEvent( EVENT_ACTIVATE, ("onActivate"), _listener );
		Eventable::registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		MousePickerAdapter::activatePicker();
		GlobalHandleAdapter::activateGlobalHandle();

		this->callEvent( EVENT_ACTIVATE, "()" );

#	ifndef MENGE_MASTER_RELEASE
		if( m_enable )
		{
			m_debugColor = 0xA0FFFFFF;
		}
		else
		{
			m_debugColor = 0x20FFFFFF;
		}

		VectorVertices::invalidateVertices();
#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		MousePickerAdapter::deactivatePicker();
		GlobalHandleAdapter::deactivateGlobalHandle();

		this->callEvent( EVENT_DEACTIVATE, "()" );

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor = 0x00000000;
		VectorVertices::invalidateVertices();
#	endif
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_enable()
	{
#	ifndef MENGE_MASTER_RELEASE
		if( m_active )
		{
			m_debugColor = 0xA0FFFFFF;
		}
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_disable()
	{
#	ifndef MENGE_MASTER_RELEASE
		if( m_active )
		{
			m_debugColor = 0x20FFFFFF;
		}
		else
		{
			m_debugColor = 0x00000000;
		}
		VectorVertices::invalidateVertices();
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		Camera2D * camera = Player::get()
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
	void HotSpot::_update( float _timing )
	{
		Node::_update( _timing );

		MousePickerAdapter::updatePicker();
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

		RenderEngine::get()
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
