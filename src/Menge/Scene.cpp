#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Layer2D.h"
#	include "Layer3D.h"

#	include "Entity.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"

#	include "ScriptEngine.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Scene);
//////////////////////////////////////////////////////////////////////////
Scene::Scene()
{

}
//////////////////////////////////////////////////////////////////////////
void Scene::layerAppend( const std::string & _layer, SceneNode2D * _node )
{
	Layer2D * layer = getChildrenT<Layer2D>( _layer );
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

	return SceneNode2D::_activate();
}
//////////////////////////////////////////////////////////////////////////
void Scene::addLayer2D( Layer2D * _layer )
{
	m_listLayer2D.push_back( _layer );
}
//////////////////////////////////////////////////////////////////////////
void Scene::addLayer3D( Layer3D * _layer )
{
	m_listLayer3D.push_back( _layer );
}
//////////////////////////////////////////////////////////////////////////
void Scene::loader(TiXmlElement *_xml)
{
	SceneNode2D::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_VALUE_NODE("Script", "File", m_scriptFile);
	}
}