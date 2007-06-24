#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Layer.h"
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
void Scene::layerAppend( const std::string & _layer, Node * _node )
{
	Layer * layer = getChildrenT<Layer>( _layer );
	if( layer )
	{
		layer->addChildren( _node );
	}
}
//////////////////////////////////////////////////////////////////////////
Node * Scene::getEntity( const std::string & _name )
{
	for( TListChildren::iterator 
		it = m_listChildren.begin(),
		it_end = m_listChildren.end();
	it != it_end;
	++it)
	{
		if( Layer * layer = dynamic_cast<Layer*>(*it) )
		{
			if( Node * node = layer->getChildren( _name ) )
			{
				return node;
			}
		}
		else
		{
			if( (*it)->getName() == _name )
			{
				return *it;
			}
		}
	}
 
	return 0;
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

	return NodeImpl::_activate();
}
//////////////////////////////////////////////////////////////////////////
void Scene::loader(TiXmlElement *_xml)
{
	NodeImpl::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_VALUE_NODE("Script", "File", m_scriptFile);
	}
}