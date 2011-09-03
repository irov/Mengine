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

#	include "LogEngine.h"

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
	void HotSpot::setPolygon( const Polygon & _polygon )
	{
		m_polygon = _polygon;

		this->endPolygon_();

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & HotSpot::getPolygon() const
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
	void HotSpot::addPoint_( const mt::vec2f & _p )
	{
		boost::geometry::append( m_polygon, _p );

		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::clearPoints()
	{
		m_polygon.clear();

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
	bool HotSpot::pick( const mt::vec2f& _point, Arrow * _arrow )
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

		if( layer->testArrow( camera, this, _point, _arrow ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD_END( Protocol::Polygon, this, &HotSpot::loaderPolygon_, &HotSpot::endPolygon_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::loaderPolygon_( BinParser * _parser )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Point_Value, &HotSpot::addPoint_ )
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::endPolygon_()
	{
		boost::geometry::correct( m_polygon );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		MousePickerAdapter::setEventListener( _listener );

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

		this->callEvent( EVENT_DEACTIVATE, "()" );

#	ifndef MENGE_MASTER_RELEASE
		m_debugColor = 0x00000000;
		VectorVertices::invalidateVertices();
#	endif
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPoint( const mt::vec2f & _p )
	{
		Camera2D * camera = Player::get()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		//const mt::vec2f & direction = this->getWorldDirection();
		//const mt::vec2f & position = this->getScreenPosition();
		const mt::box2f& myBBox = getBoundingBox();

		if( m_layer != 0 )
		{
			if( m_layer->testBoundingBox( viewport, myBBox, mt::box2f( _p, _p ) ) == false )
			{
				return false;
			}

			if( m_layer->testPoint( camera, this, _p ) == false )
			{
				return false;
			}
		}
		else
		{
			const mt::mat3f& wm = getWorldMatrix();
			
			mt::vec2f wmp;
			mt::mul_v2_m3( wmp, _p, wm );

			Polygon point_polygon;
			boost::geometry::append(point_polygon, wmp);

			//bool result = mt::is_point_inside_polygon_wm( m_polygon, _p, wm );
			bool result = boost::geometry::intersects( m_polygon, point_polygon );

			return result;
		}

		//bool result = mt::is_point_inside_polygon(m_polygon, _p, position, direction);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		std::size_t numPoints = boost::geometry::num_points( m_polygon );

		if( numPoints == 0 )
		{
			Node::_updateBoundingBox( _boundingBox );
			return;
		}

		const Polygon::ring_type & ring = m_polygon.outer();

		const mt::mat3f & wm = this->getWorldMatrix();

		mt::reset( _boundingBox, ring[0] * wm );

		for( std::size_t
			it = 1,
			it_end = numPoints;
		it != it_end; 
		++it )
		{
			mt::add_internal_point( _boundingBox, ring[it] * wm );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_update( float _timing )
	{
		Node::_update( _timing );

		MousePickerAdapter::updatePicker();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::testPolygon( const mt::mat3f& _transform, const Polygon & _screenPoly, const mt::mat3f& _screenTransform )
	{
		bool intersect = false;

		Polygon self_wm_polygon;
		polygon_wm( self_wm_polygon, m_polygon, _transform );

		std::size_t num_of_screen_poly_points = boost::geometry::num_points(_screenPoly);

		if( num_of_screen_poly_points == 1 )
		{
			const Polygon::ring_type & ring = _screenPoly.outer();

			mt::vec2f wmp;
			mt::mul_v2_m3( wmp, ring[0], _screenTransform );

			Polygon other_wm_polygon;
			boost::geometry::append(other_wm_polygon, wmp);
			
			intersect = boost::geometry::intersects( self_wm_polygon, other_wm_polygon );
		}
		else
		{
			Polygon other_wm_polygon;
			polygon_wm( other_wm_polygon, _screenPoly, _screenTransform );

			intersect = boost::geometry::intersects( self_wm_polygon, other_wm_polygon );
		}

		return intersect;
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_debugRender( Camera2D * _camera, unsigned int _debugMask )
	{
		if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) <= 0 )
		{
			return;
		}

		VectorVertices::TVectorVertex2D & vertices = this->getVertices();

		if( vertices.empty() )
		{
			return;
		}

		RenderEngine::get()
			->renderObject2D( m_debugMaterial, NULL, NULL, 0, &(vertices[0]), vertices.size(), LPT_LINE );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate )
	{
		std::size_t numpoints = boost::geometry::num_points(m_polygon);

		if( numpoints == 0 )
		{
			return;
		}

		_vertices.resize( numpoints + 1 );

		const mt::mat3f & worldMat = this->getWorldMatrix();

		const Polygon::ring_type & ring = m_polygon.outer();

		for( std::size_t i = 0; i < numpoints; ++i )
		{
			mt::vec2f trP;
			mt::mul_v2_m3( trP, ring[i], worldMat );

			_vertices[i].pos[0] = trP.x;
			_vertices[i].pos[1] = trP.y;
			_vertices[i].pos[2] = 0.f;
			_vertices[i].pos[3] = 1.f;

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
