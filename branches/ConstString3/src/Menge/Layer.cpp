#	include "Layer.h"

#	include "RenderEngine.h"

#	include "Scene.h"
#	include "HotSpot.h"
#	include "Arrow.h"

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
	bool Layer::testArrow( RenderCameraInterface * _camera, HotSpot * _layerspaceHotspot, const mt::vec2f& _point, Arrow * _arrow ) const
	{
		/*const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		const mt::vec2f & posA = _layerspaceHotspot->getScreenPosition();

		const mt::vec2f & dirB = _screenspaceHotspot->getWorldDirection();
		const mt::vec2f & posB = _screenspaceHotspot->getWorldPosition();

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();
		const mt::polygon & screenspacePolygon = _screenspaceHotspot->getPolygon();

		bool is_intersect = mt::intersect_poly_poly( 
			layerspacePolygon, 
			screenspacePolygon,
			dirA, 
			posA, 
			dirB, 
			posB 
			);

		return is_intersect;*/
		mt::mat4f lwm = _layerspaceHotspot->getWorldMatrix();

		mt::vec2f cp = _layerspaceHotspot->getCameraPosition(_camera);

		lwm.v3.x = cp.x;
		lwm.v3.y = cp.y;

		//const mt::mat3f & awm = _arrow->getWorldMatrix();
		//const mt::mat3f & acm = _arrow->getClickMatrix();

		mt::mat4f click_wm;
		mt::ident_m4(click_wm);

			//= awm;
		click_wm.v3.x = _point.x;
		click_wm.v3.y = _point.y;
		click_wm.v3.z = 0.f;
		//mt::mul_m3_m3( click_wm, awm, acm );
		
		//const Polygon & screenPoly = _arrow->getPolygon();

		//bool result = _layerspaceHotspot->testPolygon( lwm, screenPoly, click_wm );

		bool result = _layerspaceHotspot->testArrow( lwm, _arrow, click_wm );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testPoint( RenderCameraInterface * _camera, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const
	{
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
		// do nothing
	}
}
