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
: m_renderCamera(0)
{

}
//////////////////////////////////////////////////////////////////////////
void Scene::setRenderCamera( Camera * _camera)
{
	m_renderCamera = _camera;
}
//////////////////////////////////////////////////////////////////////////
Camera * Scene::getRenderCamera()
{
	return m_renderCamera;
}
//////////////////////////////////////////////////////////////////////////
bool Scene::_compile()
{
	if( m_scriptFile.empty() == false )
	{
		int Error = Keeper<ScriptEngine>::hostage()->doFile(m_scriptFile);

		return Error == 0;
	}

	return NodeImpl::_compile();
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