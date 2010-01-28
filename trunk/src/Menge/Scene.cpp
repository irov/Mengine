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
	: m_mainLayer(0)
	, m_parentScene(0)
	, m_offsetPosition(0.f,0.f)
	, m_gravity2D( 0.0f, 0.0f )
	, m_physWorldBox2D( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_physWorld2D( false )
	, m_rtName( "Window" )
	, m_rtSize( 0.0f, 0.0f )
	, m_onUpdateEvent(false)
	, m_blockInput( false )
	, m_camera2D( NULL )
	, m_scheduleManager( NULL )
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
	bool Scene::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( isActivate() == false  )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalKeyHandler::iterator
				it = m_setGlobalKeyHandler.begin(),
				it_end = m_setGlobalKeyHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		if( handle == false && m_blockInput == false )
		{
			if( updatable() )
			{
				if( askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown ) == false )
				{
					handle = false;
				}
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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
	bool Scene::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}
		
		if( handle == false && m_blockInput == false )
		{
			if( updatable() )
			{
				if( askEvent( handle, EVENT_MOUSE_BUTTON, "(Ib)", _button, _isDown ) == false )
				{
					handle = false;
				}
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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
	bool Scene::handleMouseMove( float _x, float _y, int _whell )
	{
		if( isActivate() == false )
		{
			return false;
		}

		bool handle = false;

		if( handle == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				if( handle = (*it++)->handleGlobalMouseMove( _x, _y, _whell ) )
				{
					break;
				}
			}
		}

		if( updatable() )
		{
			if( handle == false && m_blockInput == false )
			{
				if( askEvent( handle, EVENT_MOUSE_MOVE, "(ffi)", _x, _y, _whell ) == false )
				{
					handle = false;
				}
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
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

		registerEvent( EVENT_KEY, ("onHandleKeyEvent") );
		registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent") );
		registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove") );
		registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd") );
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

		if( m_physWorld2D && PhysicEngine2D::hostage()->isWorldCreate() == false )
		{
			mt::vec2f minBox( m_physWorldBox2D.x, m_physWorldBox2D.y );
			mt::vec2f maxBox( m_physWorldBox2D.z, m_physWorldBox2D.w );

			PhysicEngine2D::hostage()
				->createWorld( minBox, maxBox, m_gravity2D );
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
			XML_CASE_NODE( "PhysicWorld2DBox" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", m_physWorldBox2D );
				}
				m_physWorld2D = true;
			}
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

			if( m_physWorld2D )
			{
				mt::vec2f minBox( m_physWorldBox2D.x, m_physWorldBox2D.y );
				mt::vec2f maxBox( m_physWorldBox2D.z, m_physWorldBox2D.w );

				Holder<PhysicEngine2D>::hostage()
					->createWorld( minBox, maxBox, m_gravity2D );
			}
		}
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
	bool Scene::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if(  m_blockInput == true )
		{
			return false;
		}

		if( updatable() )
		{
			if( handle == false )
			{
				askEvent( handle, EVENT_MOUSE_BUTTON_END, "(Ib)", _button, _isDown );
			}
		}

		if( handle == false )
		{
			for( TContainerChildren::reverse_iterator 
				it = m_children.rbegin(),
				it_end = m_children.rend();
			it != it_end;
			++it)
			{
				if( handle = (*it)->handleMouseButtonEventEnd( _button, _isDown ) )
				{
					break;
				}
			}
		}

		return handle;
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
	void Scene::regGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find == m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find != m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find == m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scene::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find != m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.erase( it_find );
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