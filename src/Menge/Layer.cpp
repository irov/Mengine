#	include "Layer.h"

#	include "ObjectImplement.h"

#	include "Scene.h"
#	include "Camera2D.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Layer);
//////////////////////////////////////////////////////////////////////////
Layer::Layer()
: m_factorParallax(1.f,1.f)
{

}
//////////////////////////////////////////////////////////////////////////
void Layer::setParallaxFactor( const mt::vec2f & _factor )
{
	m_factorParallax = _factor;
}
//////////////////////////////////////////////////////////////////////////()
void Layer::setParent(Node *node)
{
	if(dynamic_cast<Scene*>(node) == 0)
	{
		return;
	}

	NodeImpl::setParent(node);
}
//////////////////////////////////////////////////////////////////////////
const Viewport & Layer::updateViewport( const Viewport & _viewPort )
{
	m_viewPort = _viewPort;
	
	m_viewPort.begin.x *= m_factorParallax.x;
	m_viewPort.begin.y *= m_factorParallax.y;

	m_viewPort.end.x *= m_factorParallax.x;
	m_viewPort.end.y *= m_factorParallax.y;

	return m_viewPort;
}
//////////////////////////////////////////////////////////////////////////
void Layer::loader( TiXmlElement *_xml)
{
	Renderable::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("Parallax")
		{
			mt::vec2f offset;
			XML_VALUE_ATTRIBUTE("Factor", offset);
			setParallaxFactor( offset );
		}
	}
}