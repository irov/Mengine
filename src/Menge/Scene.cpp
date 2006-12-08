#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"
#	include "Layer.h"

#	include "XmlParser.h"

#	include "Utility/map_algorithm.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Scene);
//////////////////////////////////////////////////////////////////////////

Scene::Scene()
	: m_playListName("")
{

}

bool Scene::addChildren(Node *_node)
{
	if( dynamic_cast<Layer*>(_node) == 0 )
	{
		assert(!"Can't add non-layer node");
		return false;
	}

	return NodeImpl::addChildren(_node);
};

void Scene::loader( TiXmlElement *_xml )
{
	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_VALUE_NODE("PlayList","Name",m_playListName);
	}
};