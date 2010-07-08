#	include "Layer.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

#	include "Scene.h"
#	include "HotSpot.h"

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
	Scene * Layer::getScene() const
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
	void Layer::loader( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Main", "Value", &Layer::setMain );
			XML_CASE_ATTRIBUTE_NODE( "Size", "Value", m_size );

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
	bool Layer::testPolygon( const Viewport & _viewport, HotSpot * _layerspaceHotspot, const mt::polygon & _polygon, const mt::mat3f & _wm ) const
	{
		mt::mat3f lwm = _layerspaceHotspot->getWorldMatrix();
		lwm.v2 = mt::vec3f( _layerspaceHotspot->getScreenPosition(), 1.0f );
		
		return _layerspaceHotspot->testPolygon( lwm, _polygon, _wm );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testPoint( const Viewport& _viewport, HotSpot* _layerspaceHotspot, const mt::vec2f& _point ) const
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		const mt::vec2f & posA = _layerspaceHotspot->getScreenPosition();

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();

		bool is_intersect = mt::is_point_inside_polygon( 
			layerspacePolygon, 
			_point,
			posA, 
			dirA
			);

		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer::calcScreenPosition( const Viewport& _viewport, Node* _node ) const
	{
		return _node->getWorldPosition() - _viewport.begin; //maybe need add??
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		// do nothing
	}
	//////////////////////////////////////////////////////////////////////////
	Camera* Layer::getCamera()
	{
		return NULL;
	}
}
