#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"
#	include "Layer.h"

#	include "ScriptEngine.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Scene);
//////////////////////////////////////////////////////////////////////////
Scene::Scene()
	: m_playListName("")
{

}
//////////////////////////////////////////////////////////////////////////
bool Scene::addChildren(Node *_node)
{
	if( dynamic_cast<Layer*>(_node) == 0 )
	{
		assert(!"Can't add non-layer node");
		return false;
	}

	return NodeImpl::addChildren(_node);
};
//////////////////////////////////////////////////////////////////////////
bool Scene::_compile()
{
	if( m_scriptFile.empty() == false )
	{
		int Error = Keeper<ScriptEngine>::hostage()->doFile(m_scriptFile);

		return Error == 0;
	}

	NodeImpl::_compile();

	return true;
};
//////////////////////////////////////////////////////////////////////////
void Scene::loader(TiXmlElement *_xml)
{
	NodeImpl::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_VALUE_NODE("Script", "File", m_scriptFile);
	}
};