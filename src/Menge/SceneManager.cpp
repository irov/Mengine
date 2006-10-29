#	include "SceneManager.h"
#	include "NodeFactory.h"

#	include "Node.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "ScriptFunction.h"

#	include "Manager/XmlManager.h"

using namespace Menge;

//////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
{
	Keeper<SceneManager>::keep(this);
}
//////////////////////////////////////////////////////////////////////////
Node* SceneManager::createNode(const std::string &_name, const std::string &_type)
{
	Node *node = NodeFactory::genNode(_type);
	
	if( node )
	{
		node->setName(_name);
	}

	return node;
}
//////////////////////////////////////////////////////////////////////////
bool SceneManager::loadNode(Node *_node, const std::string &_xml)
{
	XML_PARSE_FILE_EX(_xml )
	{
		_node->loader(XML_CURRENT_NODE);
	}
	XML_INVALID_PARSE()
	{
		return false;
	}

	return true;
}
 
#	include "Scene.h"
#	include "Layer.h"
#	include "Track.h"
#	include "Sprite.h"


static Scene *g_scene;

void init_base_scene(SceneManager *sceneMgr)
{
	//g_scene = sceneMgr->createNodeT<Scene>("Scene1","Scene");

	//Layer *layer = g_scene->createLayer("Base");

	//Track *track = sceneMgr->createNodeT<Track>("Track1","Track");
	//layer->addChildren(track);

	//track->addPoint(mt::vec2f(0,0));
	//track->addPoint(mt::vec2f(100,100));
	//track->addPoint(mt::vec2f(200,50));
	//track->addPoint(mt::vec2f(300,150));

	//Sprite *sprite1 = sceneMgr->createNodeT<Sprite>("Sprite1","Sprite");
	//track->addChildren(sprite1);

	//Sprite *sprite2 = sceneMgr->createNodeT<Sprite>("Sprite2","Sprite");
	//sprite1->addChildren(sprite2);
}