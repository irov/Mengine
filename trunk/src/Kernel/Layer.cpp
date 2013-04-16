#	include "Kernel/Layer.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Scene.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer::Layer()
		: m_main(false)
		, m_size(0.f, 0.f)
		, m_scene(0)
	{
		m_layer = this;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer::~Layer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setMain( bool _main )
	{
		m_main = _main;

		if( m_scene )
		{
			m_scene->setMainLayer( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::isMain() const
	{
		return m_main;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setScene( Scene * _scene )
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Layer::getScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setSize( const mt::vec2f & _size )
	{
		m_size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setLayer( Layer * _layer )
	{
        (void)_layer;

		Node::setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		mt::box2f convertBox = _screenspaceBox;
		convertBox.minimum += _viewport.begin;
		convertBox.maximum += _viewport.begin;

		bool result = mt::is_intersect( _layerspaceBox, convertBox );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testPoint( RenderCameraInterface * _camera, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const
	{
        (void)_camera;
        (void)_layerspaceHotspot;
        (void)_point;

		//const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		//const mt::vec2f & posA = _layerspaceHotspot->getCameraPosition(_camera2D);

		//const Polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();

		//mt::mat3f worldMatrixA;
		//mt::set_m3_from_axes( worldMatrixA, mt::vec3f(dirA,1), mt::vec3f(mt::perp(dirA),1), mt::vec3f(posA,1) );

		//Polygon layerspacePolygon_wm;
		//polygon_wm( layerspacePolygon_wm, layerspacePolygon, worldMatrixA );

		//Polygon polygon_point;
		//boost::geometry::append(polygon_point, _point);

		//bool is_intersect = boost::geometry::intersects(layerspacePolygon_wm, polygon_point);

		//return is_intersect;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const
	{
		const mt::vec3f & wp = _node->getWorldPosition();
		_screen = wp.to_vec2f() - _viewport.begin; //maybe need add??
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_updateBoundingBox( mt::box2f& _boundingBox )
	{
        (void)_boundingBox;
		// do nothing
	}
}
