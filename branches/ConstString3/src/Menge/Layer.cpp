#	include "Layer.h"

#	include "BinParser.h"
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

		m_scene->setMainLayer( this );
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
	void Layer::loader( BinParser * _parser )
	{
		Node::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Main_Value, &Layer::setMain );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Size_Value, &Layer::setSize );
		}
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
	bool Layer::testArrow( const Viewport & _viewport, HotSpot * _layerspaceHotspot, Arrow * _arrow ) const
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
		const mt::polygon & screenPoly = _arrow->getPolygon();

		mt::mat3f lwm = _layerspaceHotspot->getWorldMatrix();
		lwm.v2 = mt::vec3f( _layerspaceHotspot->getScreenPosition(), 1.0f );

		const mt::mat3f & awm = _arrow->getWorldMatrix();
		const mt::mat3f & acm = _arrow->getClickMatrix();

		mt::mat3f click_wm = awm;
		click_wm.v0.x = acm.v0.x;
		click_wm.v1.y = acm.v1.y;
		//mt::mul_m3_m3( click_wm, awm, acm );
		
		return _layerspaceHotspot->testPolygon( lwm, screenPoly, click_wm );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testPoint( const Viewport& _viewport, HotSpot* _layerspaceHotspot, const mt::vec2f& _point ) const
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		const mt::vec2f & posA = _layerspaceHotspot->getScreenPosition();

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();

		bool is_intersect = mt::is_point_inside_polygon_pd( 
			layerspacePolygon, 
			_point,
			posA, 
			dirA
			);

		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const
	{
		_screen = _node->getWorldPosition() - _viewport.begin; //maybe need add??
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		// do nothing
	}
}
