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
	: m_mainLayer(0)
	{
	}
	Scene::~Scene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
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
		bool handle = false;
		
		if( handle )
		{
			handle = Holder<ScriptEngine>::hostage()
				->handleKeyEvent( m_scriptable, _key, _isDown );
		}

		if( handle )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleKeyEvent( _key, _isDown );
		}

		return handle;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool handle = false;

		if( handle )
		{
			handle = Holder<ScriptEngine>::hostage()
				->handleMouseButtonEvent( m_scriptable, _button, _isDown );
		}

		if( handle )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleMouseButtonEvent( _button, _isDown );
		}

		return handle;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( float _x, float _y, float _whell )
	{
		bool handle = false;

		if( handle )
		{
			handle = Holder<ScriptEngine>::hostage()
				->handleMouseMove( m_scriptable, _x, _y, _whell );
		}

		if( handle )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleMouseMove( _x, _y, _whell );
		}
	
		return handle;
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
	void Scene::_update( float _timing )
	{
		Holder<ScriptEngine>::hostage()
			->callModuleFunction( m_scriptable, "onUpdate", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( TiXmlElement *_xml )
	{
		NodeCore::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Layer * _layer )
	{
		_layer->setScene( this );

		if( _layer->isMain() )
		{
			m_mainLayer = _layer;
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
