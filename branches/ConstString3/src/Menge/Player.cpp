#	include "Player.h"

#	include "NodeManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"
#	include "Consts.h"

#	include "Arrow.h"

#	include "Logger/Logger.h"

#	include "TextField.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ParticleEngine.h"

#	include "MousePickerSystem.h"
#	include "GlobalHandleSystem.h"

#	include "ScheduleManager.h"
#	include "SceneManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
		: m_scene(0)
		, m_arrow(0)
		, m_renderCamera2D(0)
		, m_switchScene(false)
		, m_destroyOldScene( false )
		, m_restartScene( false )
		, m_arrowHided( false )
		, m_setScenePyCb( NULL )
		, m_fps( 0 )
#	ifndef MENGE_MASTER_RELEASE
		, m_showDebugText( false )
		, m_debugText( NULL )
#	endif
	{
		m_mousePickerSystem = new MousePickerSystem();
		m_globalHandleSystem = new GlobalHandleSystem();
		m_scheduleManager = new ScheduleManager();
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
#	ifndef MENGE_MASTER_RELEASE
		if( m_debugText != NULL	 )
		{
			m_debugText->destroy();
			m_debugText = NULL;
		}
#	endif

		if( m_renderCamera2D )
		{
			m_renderCamera2D->destroy();
		}

		if( m_mousePickerSystem )
		{
			delete m_mousePickerSystem;
			m_mousePickerSystem = 0;
		}

		if( m_globalHandleSystem )
		{
			delete m_globalHandleSystem;
			m_globalHandleSystem = 0;
		}

		if( m_scheduleManager != NULL )
		{
			delete m_scheduleManager;
			m_scheduleManager = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentScene( const ConstString& _name, bool _destroyOld )
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
	ScheduleManager * Player::getScheduleManager()
	{
		return m_scheduleManager;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::init( const Resolution & _contentResolution )
	{
		Arrow * arrow = 
			Game::hostage()->getDefaultArrow();

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Player::init default arrow not found" );
			return false;
		}

		arrow->setWindow( _contentResolution );

		float crx = float( _contentResolution.getWidth() );
		float cry = float( _contentResolution.getHeight() );

		mt::vec2f crv( crx, cry );

		Camera2D * camera = NodeManager::hostage()
			->createNodeT<Camera2D>( Consts::c_Camera2D);

		camera->setViewportSize( crv );
		camera->setLocalPosition( crv * 0.5f );
		camera->activate();

		setRenderCamera2D( camera );
		setArrow( arrow );

#	ifndef MENGE_MASTER_RELEASE
		m_debugText = Holder<NodeManager>::hostage()->
			createNodeT<TextField>( Consts::c_TextField );

		m_debugText->setResource( Consts::c_ConsoleFont );
		m_debugText->activate();
#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleKeyEvent( _key, _char, _isDown );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleKeyEvent( m_arrow, _key, _char, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseButtonEvent( _button, _isDown );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseButtonEvent( m_arrow, _button, _isDown );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->handleMouseButtonEventEnd( _button, _isDown );
		}

		if( m_arrow )
		{
			m_mousePickerSystem->handleMouseButtonEventEnd( m_arrow, _button, _isDown );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::handleMouseMove( float _x, float _y, int _whell )
	{
		//m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( m_globalHandleSystem )
		{
			if( handler == false )
			{
				handler = m_globalHandleSystem->handleMouseMove( _x, _y, _whell );
			}
		}

		if( m_arrow )
		{
			if( handler == false )
			{
				handler = m_mousePickerSystem->handleMouseMove( m_arrow, _x, _y, _whell );
			}
		}

		return handler;
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateChangeScene()
	{
		if( m_switchScene == false )
		{
			return;
		}

		if( m_restartScene )		// just restart scene
		{		
			m_restartScene = false;

			m_nextSceneName = m_scene->getName();
		}

		if( m_arrow )
		{
			m_arrow->deactivate();
		}

		m_scheduleManager->removeAll();

		if( m_scene )
		{
			if( m_destroyOldScene )
			{
				const ConstString & name = m_scene->getName();

				if( SceneManager::hostage()
					->destroyScene( name ) == false )
				{
					m_scene->deactivate();
				}
			}
			else
			{
				m_scene->release();
			}
		}

		m_mousePickerSystem->clear();
		m_globalHandleSystem->clear();

		m_scene = 0;

		m_scene = SceneManager::hostage()
			->getScene( m_nextSceneName );

		if( m_scene == 0 )
		{
			MENGE_LOG_ERROR( "Player::updateChangeScene scene not found %s"
				, m_nextSceneName.c_str() 
				);

			return;
		}

		m_switchScene = false;

		if( m_setScenePyCb != NULL )
		{
			pybind::call( m_setScenePyCb, "(O)", m_scene->getEmbedding() );
			m_setScenePyCb = NULL;
		}

		//Holder<ResourceManager>::hostage()->_dumpResources( "before compile next sceve " + m_scene->getName() );

		m_scene->activate();
		m_arrow->activate();

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

		if( Holder<PhysicEngine2D>::hostage()->isWorldCreate() )
		{
			const mt::vec2f & arrowPos = 
				m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

			Holder<PhysicEngine2D>::hostage()
				->onMouseMove( arrowPos );
		}

		if( m_renderCamera2D )
		{
			m_renderCamera2D->update( _timing );
		}

		if( m_arrow )
		{
			m_mousePickerSystem->update( m_arrow );
		}

		if( m_scene )
		{
			m_scene->update( _timing );
		}

		m_scheduleManager->update( _timing );

		if( m_arrow )
		{
			m_arrow->update( _timing );
		}

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
		if( m_scene )
		{
			m_scene->render( m_renderCamera2D );

			if( m_arrow )
			{
				RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
				//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

				renderEngine->beginLayer2D();
				renderEngine->setRenderTarget( Consts::c_Window );
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
					//MENGE_LOG_INFO( "TextureMemory: %.2f\n", (float)redi.textureMemory / (1024*1024) );
				}
#	endif

				renderEngine->endLayer2D();
				//m_renderCamera2D->setLocalPosition( pos );
			}
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
	void Player::onFocus( bool _focus )
	{
		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onFocus( _focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentSceneCb( const ConstString& _scene, PyObject* _cb )
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
