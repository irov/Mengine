#	include "Layer.h"

#	include "ObjectImplement.h"

#	include "Scene.h"
#	include "Camera2D.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Layer);
//////////////////////////////////////////////////////////////////////////
Layer::Layer()
: m_factorParallax(1.f,1.f)
{

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
const ViewPort & Layer::updateViewPort( const ViewPort & _viewPort )
{
	m_viewPort = _viewPort;
	
	m_viewPort.begin.x *= m_factorParallax.x;
	m_viewPort.begin.y *= m_factorParallax.y;

	m_viewPort.end.x *= m_factorParallax.x;
	m_viewPort.end.y *= m_factorParallax.y;

	return m_viewPort;
}