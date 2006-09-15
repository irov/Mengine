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
Layer * Scene::createLayer(const std::string &_nameLayer)
{
	Layer *layer = 
		m_sceneManager
		->createNodeT<Layer>(_nameLayer,"Layer");
	
	addChildren(layer);

	return layer;
}
//////////////////////////////////////////////////////////////////////////
Layer * Scene::getLayer(const std::string &_nameLayer)
{
	Layer * layer = getChildrenT<Layer>(_nameLayer);

	return layer;		
}
//////////////////////////////////////////////////////////////////////////
void Scene::loader(TiXmlElement *_xml)
{
	XML_FOR_EACH_ATTRIBUTES(_xml)
	{
		XML_DEF_ATTRIBUTES_NODE(ScriptFile);
	
	}

	XML_FOR_EACH_TREE(_xml)
	{
		XML_CHECK_NODE("Layer")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);

			Layer *layer = createLayer(Name);
		}
	}
}