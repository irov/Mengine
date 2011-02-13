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
#	include "EventManager.h"

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
		m_eventManager = new EventManager();
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

		if( m_eventManager != NULL )
		{
			delete m_eventManager;
			m_eventManager = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setCurrentScene( const ConstString& _name, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb )
	{
		m_switchSceneName = _name;
		
		if( m_scene != NULL && m_switchSceneName == m_scene->getName() )
		{
			m_restartScene = true;
		}

		m_switchScene = true;

		m_destroyOldScene = _destroyOld;
		m_destroyAfterSwitch = _destroyAfterSwitch;

		m_setScenePyCb = _cb;

		if( m_setScenePyCb )
		{
			pybind::incref( m_setScenePyCb );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Player::getCurrentScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::updateChangeScene_()
	{
		if( m_switchScene == false )
		{
			return true;
		}

		Scene * oldScene = m_scene;
		m_scene = 0;

		if( m_restartScene )		// just restart scene
		{		
			m_restartScene = false;

			m_switchSceneName = oldScene->getName();
		}

		if( m_arrow )
		{
			m_arrow->removeFromParent();
			m_arrow->disable();
		}

		m_scheduleManager->removeAll();

		if( oldScene && m_destroyOldScene == true && m_destroyAfterSwitch == false )
		{
			oldScene->destroy();
		}

		m_mousePickerSystem->clear();
		m_globalHandleSystem->clear();

		m_scene = SceneManager::get()
			->createScene( m_switchSceneName );

		m_switchScene = false;

		if( oldScene && m_destroyOldScene == true && m_destroyAfterSwitch == true )
		{
			oldScene->destroy();
		}

		if( m_scene == 0 )
		{
			MENGE_LOG_ERROR( "Player::updateChangeScene scene not found %s"
				, m_switchSceneName.c_str() 
				);

			return false;
		}

		if( m_setScenePyCb != NULL )
		{
			pybind::call( m_setScenePyCb, "(O)", m_scene->getEmbed() );

			pybind::decref( m_setScenePyCb );

			m_setScenePyCb = NULL;
		}

		//Holder<ResourceManager>::get()->_dumpResources( "before compile next sceve " + m_scene->getName() );

		m_scene->enable();
		m_arrow->enable();

		//Holder<ResourceManager>::get()->_dumpResources( "after compile next sceve " + m_scene->getName() );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::isChangedScene() const
	{
		return m_switchScene;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::setArrow(Arrow * _arrow)
	{
		if( m_arrow )
		{
			m_arrow->disable();
		}

		m_arrow = _arrow;

		if( m_arrow )
		{
			m_arrow->enable();
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
	bool Player::init( Arrow * _arrow, const Resolution & _contentResolution )
	{
		if( _arrow == 0 )
		{
			MENGE_LOG_ERROR( "Player::init default arrow not found" );
			return false;
		}

		_arrow->setWindow( _contentResolution );

		float crx = float( _contentResolution.getWidth() );
		float cry = float( _contentResolution.getHeight() );

		mt::vec2f crv( crx, cry );

		Camera2D * camera = NodeManager::get()
			->createNodeT<Camera2D>( "playerCamera", Consts::get()->c_Camera2D, Consts::get()->c_builtin_empty );

		camera->setViewportSize( crv );
		camera->setLocalPosition( crv * 0.5f );
		camera->enable();

		this->setRenderCamera2D( camera );
		this->setArrow( _arrow );

#	ifndef MENGE_MASTER_RELEASE
		m_debugText = NodeManager::get()->
			createNodeT<TextField>( "debugText", Consts::get()->c_TextField, Consts::get()->c_builtin_empty );

		m_debugText->setResourceFont( Consts::get()->c_ConsoleFont );
		m_debugText->enable();
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
	void Player::tick( float _timing )
	{
		static float fpsTiming = 0.0f;
		fpsTiming += _timing;
		RenderEngine* re = Holder<RenderEngine>::get();
		if( fpsTiming >= 1000.0f )
		{
			m_fps = re->getDebugInfo().frameCount;
			re->resetFrameCount();
			while( fpsTiming >= 1000.0f ) 
			{
				fpsTiming -= 1000.0f;
			}
		}

		if( PhysicEngine2D::get()->isWorldCreate() )
		{
			const mt::vec2f & arrowPos = 
				m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

			PhysicEngine2D::get()
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool Player::update()
	{
		if( this->updateChangeScene_() == false )
		{
			return false;
		}

		if( m_globalHandleSystem )
		{
			m_globalHandleSystem->update();
		}

		if( m_eventManager )
		{
			m_eventManager->update();
		}

		return true;
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
		if( m_switchScene == true )
		{
			return;
		}

		if( m_scene == 0 )
		{
			return;
		}

		m_scene->render( m_renderCamera2D );

		RenderEngine* renderEngine = RenderEngine::get();
		//renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		renderEngine->beginLayer2D();
		renderEngine->setRenderTarget( Consts::get()->c_Window );
		renderEngine->setActiveCamera( m_renderCamera2D );

		if( m_arrow && m_arrow->hasParent() == false )
		{
			m_arrow->render( m_renderCamera2D );
		}

#	ifndef MENGE_MASTER_RELEASE
		if( m_showDebugText == true )
		{
			const RenderEngine::DebugInfo& redi = 
				RenderEngine::get()->getDebugInfo();

			//size_t particlesCount = 
			//	Holder<ParticleEngine>::get()->getFrameParticlesCount();

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
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseLeave()
	{
		if( m_arrow )
		{
			m_arrow->onAppMouseLeave();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onAppMouseLeave();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onAppMouseEnter()
	{
		if( m_arrow )
		{
			m_arrow->onAppMouseEnter();
		}

		if( m_scene && m_scene->isActivate() )
		{
			m_scene->onAppMouseEnter();
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
