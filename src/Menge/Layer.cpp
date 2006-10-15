#	include "Layer.h"
#	include "ObjectImplement.h"

#	include "Scene.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Layer);
//////////////////////////////////////////////////////////////////////////
void Layer::setParent(Node *node)
{
	if(dynamic_cast<Scene*>(node) == 0)
	{
		return;
	}

	Node::setParent(node);
}