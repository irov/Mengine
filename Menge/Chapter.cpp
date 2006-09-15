#	include "Chapter.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "Manager/XmlManager.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Chapter)
//////////////////////////////////////////////////////////////////////////()
Scene * Chapter::createScene(const std::string &_name)
{
	Scene *scene = 
		m_sceneManager
		->createNodeT<Scene>(_name,"Scene");

	addChildren(scene);

	return scene;
}
//////////////////////////////////////////////////////////////////////////
Scene * Chapter::getScene(const std::string &_name)
{
	Node *node = getChildren(_name);
	
	Scene *scene = static_cast<Scene*>(node);
	return scene;
}
//////////////////////////////////////////////////////////////////////////
void Chapter::removeScene(Scene *_scene)
{
	removeChildren(_scene);
}
//////////////////////////////////////////////////////////////////////////
void Chapter::loader(TiXmlElement *_xml)
{

};