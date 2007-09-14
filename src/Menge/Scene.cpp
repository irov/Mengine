#	include "Scene.h"
#	include "ObjectImplement.h"

#	include "Camera2D.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"
#	include "MousePickerSystem.h"

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
	Scene::~Scene()
	{
		size_t size = m_listChildren.size();
		size_t f = size;
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
		return Holder<MousePickerSystem>::hostage()
			->handleKeyEvent( _key, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return Holder<MousePickerSystem>::hostage()
			->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( float _x, float _y, float _whell )
	{
		return Holder<MousePickerSystem>::hostage()
			->handleMouseMove( _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_activate()
	{
		const std::string & name = this->getName();
		
		if( name.empty() )
		{
			return false;
		}

		Holder<ScriptEngine>::hostage()
			->callModuleFunction( m_scriptable, "onActivate" );

		return NodeCore::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( TiXmlElement *_xml )
	{
		NodeCore::loader(_xml);
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
