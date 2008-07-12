#	include "Layer.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer::Layer()
		: m_main(false)
		, m_size(0.f, 0.f)
		, m_scene(0)
		, m_renderArea( 0.0f, 0.0f, 0.0f, 0.0f )
		, m_renderTarget( "defaultCamera" )
	{}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setMain( bool _main )
	{
		m_main = _main;
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
			XML_CASE_ATTRIBUTE_NODE( "Main", "Value", m_main );
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
		m_renderTarget = _cameraName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		return false;
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
}