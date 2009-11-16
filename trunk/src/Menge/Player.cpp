#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Arrow.h"

#	include "LogEngine.h"

#	include "MousePickerSystem.h"
#	include "TextField.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"
#	include "ScheduleManager.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ParticleEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera2D(0)
	, m_switchScene(false)
	, m_destroyOldScene( false )
	, m_restartScene( false )
	, m_arrowHided( false )
	, m_mousePickerSystem( 0 )
	, m_scheduleManager( NULL )
	, m_setScenePyCb( NULL )
	, m_fps( 0 )
#	ifndef MENGE_MASTER_RELEASE
	, m_showDebugText( false )
	, m_debugText( NULL )
#	endif
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
#	ifndef MENGE_MASTER_RELEASE
		if( m_debugText != NULL	 )
		{
			m_debugText->deactivate();
			m_debugText->release();
			delete m_debugText;
			m_debugText = NULL;
		}
#	endif

		if( m_scheduleManager != NULL )
		{
			delete m_scheduleManager;
			m_scheduleManager = NULL;
		}

		if( m_renderCamera2D )
		{
			delete m_renderCamera2D;
		}

		if( m_mousePickerSystem )
		{
			Holder<MousePickerSystem>::empty();
			delete m_mousePickerSystem;
			m_mousePickerSystem = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentScene( const String& _name, bool _destroyOld )
	{
		m_nextSceneName = _name;
		if( m_scene != NULL && m_nextSceneName == m_scene->getName() )
		{
			m_restartScene = true;
		}

		m_switchScene = true;
		m_destroyOldScene = _destroyOld;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Player::getCurrentScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setArrow(Arrow * _arrow)
	{
		if( m_arrow )
		{
			m_arrow->deactivate();
		}

		m_arrow = _arrow;
		
		if( m_arrow )
		{
			m_arrow->activate();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Player::getArrow()
	{
		return m_arrow;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::init( const Resolution & _contentResolution )
	{
		Arrow * arrow = 
			Holder<Game>::hostage()->getDefaultArrow();

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Player::init default arrow not found" );
			return false;
		}

		arrow->setWindow( _contentResolution );


		/*Camera2D * camera = Holder<SceneManager>::hostage()
			->createNodeT<Camera2D>( "Camera2D" );

		if( camera == 0 )
		{
			return false;
		}*/
		//mt::vec2f vpSz( 1024, 768 );

		float crx = float( _contentResolution.getWidth() );
		float cry = float( _contentResolution.getHeight() );

		mt::vec2f crv( crx, cry );

		Camera2D* camera = new Camera2D( crv );
		camera->setViewportSize( crv );
		camera->setLocalPosition( crv * 0.5f );
		camera->activate();

		setRenderCamera2D( camera );

		m_mousePickerSystem = new MousePickerSystem();
		Holder<MousePickerSystem>::keep( m_mousePickerSystem );

		setArrow( arrow );

		m_scheduleManager = new ScheduleManager();

#	ifndef MENGE_MASTER_RELEASE
		m_debugText = Holder<SceneManager>::hostage()->
						createNodeT<TextField>( "TextField" );
		m_debugText->setResource( "ConsoleFont" );
		m_debugText->activate();
#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalKeyHandler::iterator
				it = m_setGlobalKeyHandler.begin(),
				it_end = m_setGlobalKeyHandler.end();
			it != it_end;
			++it)
			{
				GlobalKeyHandler * keyHandler = *it;
				if( handler = keyHandler->handleGlobalKeyEvent( _key, _char, _isDown ) )
				{
					break;
				}
			}
		}

		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleKeyEvent( _key, _char, _isDown );
			}
		}

		if( handler == false )
		{
			HotSpot* picker = m_arrow->getCurrentHotSpot();
			handler = m_mousePickerSystem->handleKeyEvent( picker, _key, _char, _isDown );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				//GlobalMouseHandler * mouseHandler = *it;
				//if( handler = mouseHandler->handleGlobalMouseButtonEvent( _button, _isDown ) )
				if( handler = (*it++)->handleGlobalMouseButtonEvent( _button, _isDown ) )
				{
					break;
				}
			}
		}

		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleMouseButtonEvent( _button, _isDown );
			}
		}

		if( handler == false )
		{
			HotSpot* picker = m_arrow->getCurrentHotSpot();
			handler = m_mousePickerSystem->handleMouseButtonEvent( picker, _button, _isDown );
		}

		if( m_scene )
		{
			m_scene->handleMouseButtonEventEnd( _button, _isDown );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( float _x, float _y, int _whell )
	{
		//m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			/*++it*/)
			{
				//GlobalMouseHandler * mouseHandler = *it;
				if( handler = (*it++)->handleGlobalMouseMove( _x, _y, _whell ) )
				{
					break;
				}
			}
		}


		if( handler == false )
		{
			if( m_scene )
			{
				handler = m_scene->handleMouseMove( _x, _y, _whell );
			}
		}

		if( handler == false )
		{
			HotSpot* picker = m_arrow->getCurrentHotSpot();
			handler = m_mousePickerSystem->handleMouseMove( picker, _x, _y, _whell );
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::regGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find == m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = std::find( m_setGlobalMouseHandler.begin(), m_setGlobalMouseHandler.end(), _handler );
		if( it_find != m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find == m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.push_back( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = std::find( m_setGlobalKeyHandler.begin(), m_setGlobalKeyHandler.end(), _handler );
		if( it_find != m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateChangeScene()
	{
		if( m_switchScene == false )
		{
			return;
		}

		scheduleRemoveAll();

		m_switchScene = false;

		if( m_restartScene )		// just restart scene
		{		
			m_scheduleManager->update( 0.0f );

			m_restartScene = false;

			m_nextSceneName = m_scene->getName();
		}

		if( m_scene )
		{
			m_scene->deactivate();
			m_scene->release();

			if( m_destroyOldScene )
			{
				Holder<Game>::hostage()->destroyScene( m_scene );
			}
		}	

		m_scene = 0;

		m_scene = Holder<Game>::hostage()->getScene( m_nextSceneName );

		if( m_scene == 0 )
		{
			MENGE_LOG_ERROR( "Player::updateChangeScene scene not found %s"
				, m_nextSceneName.c_str() 
				);

			return;
		}
		
		if( m_setScenePyCb != NULL )
		{
			pybind::call( m_setScenePyCb, "(O)", m_scene->getEmbedding() );
			m_setScenePyCb = NULL;
		}

		//Holder<ResourceManager>::hostage()->_dumpResources( "before compile next sceve " + m_scene->getName() );

		m_scene->compile();
		m_scene->activate();

		//Holder<ResourceManager>::hostage()->_dumpResources( "after compile next sceve " + m_scene->getName() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::isChangedScene() const
	{
		return m_switchScene;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::tick( float _timing )
	{
		static float fpsTiming = 0.0f;
		fpsTiming += _timing;
		RenderEngine* re = Holder<RenderEngine>::hostage();
		if( fpsTiming >= 1000.0f )
		{
			m_fps = re->getDebugInfo().frameCount;
			re->resetFrameCount();
			while( fpsTiming >= 1000.0f ) 
			{
				fpsTiming -= 1000.0f;
			}
		}
		HotSpot* picker = m_arrow->getCurrentHotSpot();
		m_mousePickerSystem->update( picker );

		const mt::vec2f & arrowPos = m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;
		Holder<PhysicEngine2D>::hostage()->onMouseMove( arrowPos );

		if( m_renderCamera2D )
		{
			m_renderCamera2D->update( _timing );
		}

		if( m_scene )
		{
			m_scene->update( _timing );
		}

		if( m_arrow )
		{
			m_arrow->update( _timing );
		}

		m_scheduleManager->update( _timing );

		for( TCallbackInfoVector::iterator it = m_callbacks.begin(), it_end = m_callbacks.end();
			it != it_end;
			++it )
		{
			CallbackInfo& cbInfo = (*it);
			pybind::call( cbInfo.callback, "(Ob)", cbInfo.node, cbInfo.endFlag );
			pybind::decref( cbInfo.callback );
			pybind::decref( cbInfo.node );
		}

		m_callbacks.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setRenderCamera2D( Camera2D * _camera)
	{
		m_renderCamera2D = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D * Player::getRenderCamera2D()
	{
		return m_renderCamera2D;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCamera2DPosition( const mt::vec2f & _pos )
	{
		if( m_renderCamera2D )
		{
			m_renderCamera2D->setLocalPosition( _pos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::render( unsigned int _debugMask )
	{
		Holder<ParticleEngine>::hostage()
			->beginFrame();

		if( m_scene )
		{
			m_scene->render( m_renderCamera2D );
		}

		if( m_arrow )
		{
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

			renderEngine->beginLayer2D();
			renderEngine->setRenderTarget( "Window" );
			renderEngine->setActiveCamera( m_renderCamera2D );

			m_arrow->render( m_renderCamera2D );

#	ifndef MENGE_MASTER_RELEASE
			if( m_showDebugText == true )
			{
				const RenderEngine::DebugInfo& redi = 
					Holder<RenderEngine>::hostage()->getDebugInfo();

				//size_t particlesCount = 
				//	Holder<ParticleEngine>::hostage()->getFrameParticlesCount();

				size_t particlesCount = 0;

				char charBuffer[100];
				sprintf( charBuffer, "FPS: %d\nDIP: %d\nTexture Memory Usage: %.2f MB\nParticles: %d",
					m_fps, redi.dips, (float)redi.textureMemory / (1024*1024), particlesCount );
				m_debugText->setText( charBuffer );
				m_debugText->render( m_renderCamera2D );
				//MENGE_LOG( "TextureMemory: %.2f\n", (float)redi.textureMemory / (1024*1024) );
			}
#	endif

			renderEngine->endLayer2D();
			//m_renderCamera2D->setLocalPosition( pos );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onMouseLeave()
	{
		if( m_arrow )
		{
			m_arrow->onMouseLeave();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onMouseEnter()
	{
		if( m_arrow )
		{
			m_arrow->onMouseEnter();
		}
		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onMouseEnter();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Player::schedule( float _timing, PyObject * _func )
	{
		if( ( m_restartScene || m_switchScene ) == true )
		{
			return 0;
		}

		return m_scheduleManager->schedule( _timing, _func );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::scheduleRemove( std::size_t _id )
	{
		m_scheduleManager->remove( _id );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::scheduleRemoveAll()
	{
		m_scheduleManager->removeAll();
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::scheduleFreeze( std::size_t _id, bool _freeze )
	{
		m_scheduleManager->freeze( _id, _freeze );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::scheduleSetUpdatable( bool _updatable )
	{
		m_scheduleManager->setUpdatable( _updatable );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onFocus( bool _focus )
	{
		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onFocus( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentSceneCb( const String& _scene, PyObject* _cb )
	{
		m_setScenePyCb = _cb;
		setCurrentScene( _scene, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::addCallback( PyObject* _callback, PyObject* _node, bool _endFlag )
	{
		pybind::incref( _node );
		pybind::incref( _callback );
		CallbackInfo cbInfo = { _callback, _node, _endFlag };
		m_callbacks.push_back( cbInfo );
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCursorMode( bool _mode )
	{
		if( m_arrow != NULL )
		{
			m_arrow->setCursorMode( _mode );
		}
	}
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void Player::toggleDebugText()
	{
		m_showDebugText = !m_showDebugText;
	}
	//////////////////////////////////////////////////////////////////////////
#	endif
}
