#	include "Scene.h"

#	include "ScriptEngine.h"
#	include "XmlEngine.h"
#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"

#	include "Layer2D.h"
#	include "Player.h"
#	include "Camera2D.h"
#	include "Application.h"

#	include "LayerScene.h"

#	include "LogEngine.h"
#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "Game.h"

#	include "ScheduleManager.h"

#	include "SceneManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scene::Scene()
	: MousePickerAdapter(false)
	, m_mainLayer(0)
	, m_parentScene(0)
	, m_offsetPosition(0.f,0.f)
	, m_gravity2D(0.0f, 0.0f)
	, m_physWorldBox2D(0.0f, 0.0f, 0.0f, 0.0f)
	, m_physWorld2D(false)
	, m_rtName("Window")
	, m_rtSize(0.0f, 0.0f)
	, m_onUpdateEvent(false)
	, m_blockInput(false)
	, m_camera2D(NULL)
	, m_scheduleManager(NULL)
	{
		const Resolution& res = Game::hostage()
			->getContentResolution();

		m_camera2D = Holder<SceneManager>::hostage()->createNodeT<Camera2D>("Camera2D");
		m_camera2D->setViewportSize( mt::vec2f( res[0], res[1] ) );

		Holder<Player>::hostage()->getRenderCamera2D()
			->addChildren( m_camera2D );

		m_scheduleManager = new ScheduleManager();
	}
	//////////////////////////////////////////////////////////////////////////
	Scene::~Scene()
	{
		if( m_scheduleManager != NULL )
		{
			delete m_scheduleManager;
			m_scheduleManager = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setMainLayer( Layer * _layer )
	{
		m_mainLayer = _layer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setParentScene( Scene * _scene )
	{
		m_parentScene = _scene;

		callMethod( ("onSubScene"), "(O)", _scene->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::isSubScene() const
	{
		return m_parentScene != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getMainLayer()
	{
		return m_mainLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Scene::getNode(const String& _name )
	{
		Node * node = getChildren( _name, true );

		return node;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::onEnter()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onLeave()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::pick( Arrow * _arrow )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_pickerActive() const
	{
		if( this->getBlockInput() == true )
		{
			return false;
		}

		if( m_updatable == false )
		{
			return false;
		}

		if( this->getUpdatable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Scene::getLayerSize( const String& _name )
	{
		Layer * layer = getLayer_( _name );

		if( layer == 0 )
		{
			MENGE_LOG_ERROR( "Error: '%s' layer not found. get size", _name.c_str() );

			return mt::vec2f::zero_v2;
		}

		return layer->getSize();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerAppend( const String& _layer, Node * _node )
	{
		if( _node == NULL )
		{
			MENGE_LOG_ERROR( "Warning: appending NULL node to layer '%s'", _layer.c_str() );
			return;
		}

		Layer * layer = getLayer_( _layer );

		if( layer == 0 )
		{
			MENGE_LOG_ERROR( "Error: '%s' layer not found. Appending ignored", _layer.c_str() );
			
			return;
		}

		layer->addChildren( _node );
	}
	//////////////////////////////////////////////////////////////////////////
	Layer * Scene::getLayer_( const String& _name )
	{
		Node * children = getChildren( _name, false );

		Layer * layer = dynamic_cast<Layer*>(children);

		return layer;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerHide( const String& _layer, bool _value )
	{
		Layer * layer = getLayer_( _layer );
		
		if( layer == NULL )
		{
			MENGE_LOG_ERROR( "Error: '%s' layer not found. hide", _layer.c_str() );

			return;
		}

		layer->hide( _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_destroy()
	{
		Node::_destroy();

		if( m_camera2D != NULL )
		{
			//Holder<Player>::hostage()->getRenderCamera2D()
			//	->removeChildren( m_camera2D );
			m_camera2D->destroy();
			m_camera2D = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_activate()
	{
		const String& name = this->getName();
		
		if( name.empty() )
		{
			return false;
		}

		m_onUpdateEvent = registerEvent( EVENT_UPDATE, ("onUpdate") );

		PyObject * obj = this->getEmbedding();
		pybind::decref( obj );

		MousePickerAdapter::regEvent( obj );

		registerEvent( EVENT_LEAVE, ("onMouseLeave") );
		registerEvent( EVENT_ENTER, ("onMouseEnter") );
		registerEvent( EVENT_FOCUS, ("onFocus") );

		// scene must be already active on onActivate event
		m_active = Node::_activate();
		m_camera2D->activate();

		callMethod( ("onActivate"), "()" );

		//bool result = Node::_activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_deactivate()
	{
		m_scheduleManager->removeAll();

		callMethod( ("onDeactivate"), "()" );

		m_camera2D->deactivate();

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::compile()
	{
		if( this->isCompile() == true )
		{
			return true;
		}

		if( PhysicEngine2D::hostage()->isWorldCreate() == false )
		{
			if( createPhysicsWorld() == false )
			{
				return false;
			}
		}

		return Node::compile();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::_compile()
	{
		if( m_mainLayer )
		{
			if( m_mainLayer == NULL )
			{
				MENGE_LOG_ERROR( "Main Layer is NULL in scene '%s'"
					, m_name.c_str() 
					);

				return false;
			}

			mt::vec2f mainSize = m_mainLayer->getSize();

			Camera2D * camera2D = Holder<Player>::hostage()
				->getRenderCamera2D();

			const Viewport & viewport = camera2D->getViewport();

			mt::vec2f viewport_size = viewport.end - viewport.begin;
			//assert( viewport_size.x >= 1024.0f );

			mainSize -= viewport_size;

			//if( fabsf( mainSize.x /* mainSize.y*/) > 0.0001f )
			{
				for( TContainerChildren::reverse_iterator 
					it = m_children.rbegin(),
					it_end = m_children.rend();
				it != it_end;
				++it)
				{
					if( Layer2D * layer2D = dynamic_cast<Layer2D*>( *it ) )
					{
						mt::vec2f layerSize = layer2D->getSize();

						layerSize -= viewport_size;

						float factorX = ( mainSize.x > 0.001f ) ? ( layerSize.x / mainSize.x ) : 0.0f;
						float factorY = ( mainSize.y > 0.001f ) ? ( layerSize.y / mainSize.y ) : 0.0f;

						mt::vec2f parallaxFactor( factorX, factorY );

						layer2D->setParallaxFactor( parallaxFactor );
					}
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_release()
	{
		if( m_physWorld2D )
		{
			PhysicEngine2D::hostage()->destroyWorld();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_update( float _timing )
	{	
		// update physics first
		if( m_physWorld2D )
		{
			Holder<PhysicEngine2D>::hostage()->update( _timing );
		}

		MousePickerAdapter::updatePicker();

		Node::_update( _timing );
		//m_camera2D->update( _timing );
		if( m_onUpdateEvent )
		{
			callEvent( EVENT_UPDATE, "(f)", _timing );
		}

		m_scheduleManager->update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loader( XmlElement *_xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Scene" )
			{
				XML_PARSE_ELEMENT( this, &Scene::loaderScene_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::loaderScene_( XmlElement *_xml )
	{
		std::string name;

		Node::loader(_xml);
		
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Gravity2D", "Value", m_gravity2D );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "PhysicWorld2DBox", "Value", &Scene::setPhysicsWorld )

			XML_CASE_NODE( "RenderTarget" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", m_rtName );
					XML_CASE_ATTRIBUTE( "Size", m_rtSize );
				}
			}
		}
		XML_END_NODE()
		{
			callMethod( ("onLoader"), "()" );


		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setPhysicsWorld( const mt::vec4f & _box )
	{
		m_physWorldBox2D = _box;
		m_physWorld2D = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::createPhysicsWorld()
	{
		if( m_physWorld2D == false )
		{
			return true;
		}
		
		mt::vec2f minBox( m_physWorldBox2D.x, m_physWorldBox2D.y );
		mt::vec2f maxBox( m_physWorldBox2D.z, m_physWorldBox2D.w );

		Holder<PhysicEngine2D>::hostage()
			->createWorld( minBox, maxBox, m_gravity2D );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_addChildren( Node * _node )
	{
		if( Layer * layer = dynamic_cast<Layer*>(_node) )
		{
			layer->setScene( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setRenderTarget( const String& _cameraName, const mt::vec2f& _size )
	{
		m_rtName = _cameraName;
		m_rtSize = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::render( Camera2D * _camera )
	{
		if( isRenderable() == false )
		{
			return;
		}

		const mt::vec2f& main_size = m_mainLayer->getSize();

		Camera2D * camera2D = Holder<Player>::hostage()->getRenderCamera2D();

		mt::vec2f camPos = camera2D->getLocalPosition();
		const Viewport & vp = camera2D->getViewport();

		mt::vec2f vp_size = vp.end - vp.begin;

		if( ( camPos.y - vp_size.y * 0.5f ) < 0.0f )
		{
			camera2D->setLocalPosition( mt::vec2f( camPos.x, vp_size.y * 0.5f ) );
		}
		else if( ( camPos.y + vp_size.y * 0.5f ) > main_size.y )
		{
			camera2D->setLocalPosition( mt::vec2f( camPos.x, main_size.y - vp_size.y * 0.5f ) );
			//viewport.begin.y = main_size.y - viewport_size.y;
		}

		_render( camera2D );

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			if( (*it)->isRenderable() == false ) 
			{
				continue;
			}

			Holder<RenderEngine>::hostage()
				->setRenderTarget( m_rtName );

			(*it)->render( m_camera2D );
		}

		Camera2D * renderCamera = Holder<Player>::hostage()->getRenderCamera2D();
		const mt::vec2f & pos = renderCamera->getLocalPosition();

		if( cmp_v2_v2(pos, camPos) == false )
		{
			Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( camPos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::renderSelf()
	{
		render( m_camera2D );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseLeave()
	{
		bool handle = false;
		
		if( askEvent( handle, EVENT_LEAVE, "()" ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TContainerChildren::iterator 
				it = m_children.begin(), 
				it_end = m_children.end();
				it != it_end;
				it++ )
			{
				LayerScene* layerScene = dynamic_cast<LayerScene*>( *it );
				if( layerScene != 0 )
				{
					layerScene->onMouseLeave();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onMouseEnter()
	{
		bool handle = false;

		if( askEvent( handle, EVENT_ENTER, "()" ) )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
				it != it_end;
				it++ )
			{
				LayerScene* layerScene = dynamic_cast<LayerScene*>( *it );
				if( layerScene != 0 )
				{
					layerScene->onMouseEnter();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Scene::getRenderTarget() const
	{
		return m_rtName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::blockInput( bool _block )
	{
		m_blockInput = _block;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scene::getBlockInput() const
	{
		return m_blockInput;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::layerRemove( Node* _node )
	{
		for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
			it != it_end;
			it++ )
		{
			if( (*it)->isChildren( _node, false ) )
			{
				(*it)->removeChildren( _node );
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Scene::screenToLocal( const String& _layerName, const mt::vec2f& _point )
	{
		Layer * layer = getLayer_( _layerName );
		Layer2D* layer2D = static_cast<Layer2D*>( layer );
		return layer2D->screenToLocal( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::onFocus( bool _focus )
	{
		bool handle = false;

		if( askEvent( handle, EVENT_FOCUS, "(b)", _focus ) == false )
		{
			handle = false;
		}

		if( handle == false )
		{
			for( TContainerChildren::iterator it = m_children.begin(), it_end = m_children.end();
				it != it_end;
				it++ )
			{
				LayerScene* layerScene = dynamic_cast<LayerScene*>( *it );
				if( layerScene != 0 )
				{
					layerScene->onFocus( _focus );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::_render( Camera2D * _camera )
	{
		// nothing
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D* Scene::getCamera()
	{
		return m_camera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager * Scene::getScheduleManager()
	{
		return m_scheduleManager;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraPosition( float _x, float _y )
	{
		m_camera2D->setLocalPositionInt( mt::vec2f( _x, _y ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::enableCameraFollowing( bool _enable, float _force )
	{
		m_camera2D->enableTargetFollowing( _enable, _force );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraTarget( Node* _target )
	{
		m_camera2D->setTarget( _target );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::setCameraBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
	{
		m_camera2D->setBounds( _leftUpper, _rightLower );
	}
	//////////////////////////////////////////////////////////////////////////
}