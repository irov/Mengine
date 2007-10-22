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
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;	
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Scene::getNode(const std::string & _name )
	{
		Node * node = getChildren( _name, true );
		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerAppend( const std::string & _layer, Node * _node )
	{
		Layer * layer = getChildrenT<Layer>( _layer, false );
		if( layer )
		{
			layer->addChildren( _node );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleKeyEvent( size_t _key, bool _isDown )
	{
		bool handle = false;
		
		if( handle == false )
		{
			askEvent( handle, "KEY", "(Ib)", _key, _isDown );
		}

		if( handle == false )
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

		if( handle == false )
		{
			askEvent( handle, "MOUSE_BUTTON", "(Ib)", _button, _isDown );
		}

		if( handle == false )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleMouseButtonEvent( _button, _isDown );
		}

		return handle;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( int _x, int _y, int _whell )
	{
		bool handle = false;

		if( handle == false )
		{
			askEvent( handle, "MOUSE_MOVE", "(fff)", _x, _y, _whell );
		}

		if( handle == false )
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

		registerEventMethod( "UPDATE", "onUpdate" );

		registerEventMethod( "KEY", "onHandleKeyEvent" );
		registerEventMethod( "MOUSE_BUTTON", "onHandleMouseButtonEvent" );
		registerEventMethod( "MOUSE_MOVE", "onHandleMouseMove" );

		callMethod( "onActivate", "() " );

		return NodeCore::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( size_t _timing, const Viewport & _viewport )
	{
		callEvent( "UPDATE", "(k)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( TiXmlElement *_xml )
	{
		NodeCore::loader(_xml);

		callMethod( "onLoader", "()" );
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
