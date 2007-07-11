#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Camera2D.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"

#	include "ScriptEngine.h"

#	include "XmlParser/XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Scene);
//////////////////////////////////////////////////////////////////////////
Scene::Scene()
{

}
//////////////////////////////////////////////////////////////////////////
void Scene::layerAppend( const std::string & _layer, Node * _node )
{
	Layer * layer = getChildren( _layer );
	if( layer )
	{
		layer->addChildren( _node );
	}
}
//////////////////////////////////////////////////////////////////////////
bool Scene::_activate()
{
	if( m_scriptFile.empty() == false )
	{
		if( Holder<ScriptEngine>::hostage()->doFile(m_scriptFile) == false )
		{
			printf( "False parse script file '%s'", m_scriptFile.c_str() );
		}		
	}

	const std::string & name = getName();

	if( name.empty() == false )
	{
		std::string functionName = "Scene" + getName() + "_activate";

		Holder<ScriptEngine>::hostage()
			->callFunctionNode( functionName, this );
	}

	return NodeCore::_activate();
}
//////////////////////////////////////////////////////////////////////////
void Scene::loader( TiXmlElement *_xml )
{
	NodeCore::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_VALUE_NODE("Script", "File", m_scriptFile);
	}
}
//////////////////////////////////////////////////////////////////////////
void Scene::render()
{
	for each( Layer * layer in m_listChildren )
	{
		layer->renderLayer();
	}
};