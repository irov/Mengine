#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Camera2D.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"

#	include "ScriptEngine.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
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
	bool Scene::handleKeyEvent( size_t _key, bool _isDown )
	{
		for( TListChildren::reverse_iterator 
			it = m_listChildren.rbegin(),
			it_end = m_listChildren.rend();
		it != it_end;
		++it)
		{
			if( (*it)->handleKeyEvent( _key, _isDown ) )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		for( TListChildren::reverse_iterator 
			it = m_listChildren.rbegin(),
			it_end = m_listChildren.rend();
		it != it_end;
		++it)
		{
			if((*it)->handleMouseButtonEvent( _button, _isDown ) )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( float _x, float _y, float _whell )
	{
		for( TListChildren::reverse_iterator 
			it = m_listChildren.rbegin(),
			it_end = m_listChildren.rend();
		it != it_end;
		++it)
		{
			if( (*it)->handleMouseMove( _x, _y, _whell ) )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_activate()
	{
		m_sceneModule = Holder<ScriptEngine>::hostage()
			->importModule( m_name + "." + m_scriptFile );

		const std::string & name = this->getName();

		if( name.empty() == false )
		{
			std::string functionName = "Scene" + getName() + "_activate";

			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_sceneModule, functionName, "(O)", this->getScriptable() );
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
	}
}
