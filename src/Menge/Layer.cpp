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
const mt::mat3f & Layer::getWorldMatrix()
{
	Camera2D * camera = 
		getParentT<Scene>()
		->getRenderCamera();

	if( camera == 0 )
	{
		return Allocator2D::getWorldMatrix();
	}

	m_matrixParalax = camera->getWorldMatrix();
	scale_m3( m_matrixParalax, mt::vec3f(m_factorParallax.x, m_factorParallax.y, 1.f) );
	return m_matrixParalax;
}
