#	include "Layer.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

#	include "Scene.h"
#	include "HotSpot.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer::Layer()
		: m_main( false )
		, m_size( 0.f, 0.f )
		, m_scene( 0 )
		, m_renderArea( 0.0f, 0.0f, 0.0f, 0.0f )
	{}
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
	void Layer::loader( XmlElement * _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Main", "Value", &Layer::setMain );
			XML_CASE_ATTRIBUTE_NODE( "Size", "Value", m_size );
			XML_CASE_ATTRIBUTE_NODE( "RenderArea", "Value", m_renderArea );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setOffsetPosition( const mt::vec2f & _offset )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setRenderTarget( const std::string& _cameraName )
	{
		//m_renderTarget = _cameraName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setRenderArea( const mt::vec4f& _renderArea )
	{
		m_renderArea = _renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Layer::getRenderArea() const
	{
		return m_renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_render( unsigned int _debugMask )
	{
		Holder<RenderEngine>::hostage()
			->setRenderArea( m_renderArea );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		mt::box2f convertBox = _screenspaceBox;
		convertBox.min += _viewport.begin;
		convertBox.max += _viewport.begin;

		bool result = mt::is_intersect( _layerspaceBox, convertBox );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, HotSpot * _screenspaceHotspot ) const
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
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

		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::testHotspot( const Viewport& _viewport, HotSpot* _layerspaceHotspot, const mt::vec2f& _point ) const
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
		return _node->getWorldPosition() - _viewport.begin;
	}
	//////////////////////////////////////////////////////////////////////////
}