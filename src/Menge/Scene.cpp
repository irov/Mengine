#	include "Scene.h"

#	include "SceneManager.h"
#	include "ResourceManager.h"
#	include "MousePickerSystem.h"

#	include "ScriptEngine.h"
#	include "PhysicEngine.h"
#	include "Application.h"


#	include "XmlEngine.h"

#	include "Entity3d.h"
#	include "Camera3d.h"
#	include "Light.h"
#	include "Actor.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
	: m_mainLayer(0)
	, m_isSubScene(false)
	, m_offsetPosition(0.f,0.f)
	, m_g(0.0f, -9.8f, 0.0f)
	, m_restitution(0)
	, m_staticFriction(0)
	, m_dynamicFriction(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setParentScene( Scene * _scene )
	{
		callMethod( "onSubScene", "(O)", _scene->getScript() );

		m_isSubScene = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_isSubScene;
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
	const mt::vec2f & Scene::getLayerSize( const std::string & _name )
	{
		Layer * layer = getChildrenT<Layer>( _name, false );
		if( layer )
		{
			return layer->getSize();
		}

		return mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::actorAppend( SceneNode3D * _node )
	{
		m_listActors.push_back( _node );
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
	bool Scene::handleKeyEvent( size_t _key, size_t _char, bool _isDown )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;
		
		if( handle == false )
		{
			askEvent( handle, "KEY", "(IIb)", _key, _char, _isDown );
		}

		if( handle == false )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleKeyEvent( _key, _char, _isDown );
		}

		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			askEvent( handle, "MOUSE_BUTTON", "(Ib)", _button, _isDown );
		}

		if( handle == false  && !m_isSubScene )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleMouseButtonEvent( _button, _isDown );
		}

		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::handleMouseMove( int _x, int _y, int _whell )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			askEvent( handle, "MOUSE_MOVE", "(iii)", _x, _y, _whell );
		}

		if( handle == false )
		{
			handle = Holder<MousePickerSystem>::hostage()
				->handleMouseMove( _x, _y, _whell );
		}
	
		if( handle == false )
		{
			for( TListChildren::reverse_iterator 
				it = m_listChildren.rbegin(),
				it_end = m_listChildren.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseMove( _x, _y, _whell ) )
				{
					break;
				}
			}
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

		Camera3D * cam3d = this->m_mapCameras.begin()->second;

		activateLights_();
		activateCameras_();
		setPhysicParams_();

		callMethod( "onActivate", "() " );

		return NodeCore::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::activateLights_()
	{
		for( TMapLight::iterator
			it = m_mapLights.begin(),
			it_end = m_mapLights.end();
		it != it_end;
		++it)
		{
			it->second->activate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::activateCameras_()
	{
		for( TMapCamera::iterator
			it = m_mapCameras.begin(),
			it_end = m_mapCameras.end();
		it != it_end;
		++it)
		{
			it->second->activate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_deactivate()
	{
		callMethod( "onDeactivate", "() ");
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( float _timing )
	{

		for( TListActors::iterator
			it = m_listActors.begin(),
			it_end = m_listActors.end();
		it != it_end;
		++it)
		{
			(*it)->update( _timing );
		}

		callEvent( "UPDATE", "(f)", _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( XmlElement *_xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Scene")
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderScene_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderScene_( XmlElement *_xml )
	{
		std::string name;

		NodeCore::loader(_xml);
		NodeRenderable::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "OffsetPosition", "Value", m_offsetPosition );
			XML_CASE_ATTRIBUTE_NODE( "Gravity", "Value", m_g );
			XML_CASE_ATTRIBUTE_NODE( "Restitution", "Value", m_restitution );
			XML_CASE_ATTRIBUTE_NODE( "StaticFriction", "Value", m_staticFriction );
			XML_CASE_ATTRIBUTE_NODE( "DynamicFriction", "Value", m_dynamicFriction );
		
			XML_CASE_NODE("Entities")
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderEntities_ );
			}

			XML_CASE_NODE("Lights")
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderLights_ );
			}

			XML_CASE_NODE("Cameras")
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderCameras_ );
			}
		}
		XML_END_NODE()
		{
			callMethod( "onLoader", "()" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderCameras_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Camera")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				Camera3D * camera = new Camera3D();

				camera->setName( name );
				camera->setType( "Camera3D" );

				addCamera( camera );

				XML_PARSE_ELEMENT( camera, &Camera3D::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderLights_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Light")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				Light * light = new Light();

				light->setName( name );
				light->setType( "Light" );

				addLight( light );

				XML_PARSE_ELEMENT( light, &Light::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderEntities_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Entity")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				Entity3d * entity = new Entity3d();

				entity->setName( name );
				entity->setType( "Entity3d" );

				addEntity( entity );

				XML_PARSE_ELEMENT( entity, &Entity3d::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::addCamera( Camera3D * _camera )
	{
		const std::string & name = _camera->getName();

		TMapCamera::iterator it_find = m_mapCameras.find( name );

		if( it_find == m_mapCameras.end() )
		{
			m_mapCameras.insert( std::make_pair( name, _camera ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D * Scene::getCamera( const std::string & _name )
	{
		TMapCamera::const_iterator it_find = m_mapCameras.find( _name );

		if( it_find == m_mapCameras.end() )
		{
			return NULL;
		}

		Camera3D * camera = it_find->second;

		return camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::addLight( Light * _light )
	{
		const std::string & name = _light->getName();

		TMapLight::iterator it_find = m_mapLights.find( name );

		if( it_find == m_mapLights.end() )
		{
			m_mapLights.insert( std::make_pair( name, _light ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::addEntity( Entity3d * _entity )
	{
		const std::string & name = _entity->getName();

		TMapEntity::iterator it_find = m_mapEntities.find( name );

		if( it_find == m_mapEntities.end() )
		{
			m_mapEntities.insert( std::make_pair( name, _entity ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Entity3d * Scene::getEntity( const std::string & _name )
	{
		TMapEntity::const_iterator it_find = m_mapEntities.find( _name );

		if( it_find == m_mapEntities.end() )
		{
			return NULL;
		}

		Entity3d * entity = it_find->second;

		entity->activate();

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Layer * _layer )
	{
		_layer->setOffsetPosition(m_offsetPosition);
		_layer->setScene( this );

		if( _layer->isMain() )
		{
			m_mainLayer = _layer;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setPhysicParams_()
	{
		bool usePhysic = Holder<Application>::hostage()->usePhysic();

		if( usePhysic == true )
		{
			Holder<PhysicEngine>::hostage()->setGravity( m_g );
			Holder<PhysicEngine>::hostage()->setRestitution( m_restitution );
			Holder<PhysicEngine>::hostage()->setStaticFriction( m_staticFriction );
			Holder<PhysicEngine>::hostage()->setDynamicFriction( m_dynamicFriction );		
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
