#	include "Layer.h"

#	include "ObjectImplement.h"

#	include "Scene.h"

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
bool Layer::_updateMatrix()
{
	mt::mat3f & lwm = getLocalMatrix();

	scale_m3( lwm , mt::vec3f( m_factorParallax.x, m_factorParallax.y, 1.f ) );

	return true;
}