#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Layer.h"

#	include "SceneManager.h"

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
bool Scene::_compile()
{
	if( m_scriptFile.empty() == false )
	{
		int Error = Holder<ScriptEngine>::hostage()->doFile(m_scriptFile);

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