#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"
#	include "Layer.h"

#	include "Manager/XmlManager.h"

#	include "Utility/map_algorithm.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Scene);
//////////////////////////////////////////////////////////////////////////
bool Scene::addChildren(Node *_node)
{
	if( dynamic_cast<Layer*>(_node) == 0 )
	{
		return false;
	}

	return Node::addChildren(_node);
};
//////////////////////////////////////////////////////////////////////////
void Scene::start()
{
	activate();
}

