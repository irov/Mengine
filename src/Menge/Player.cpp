#	include "Player.h"

#	include "SceneManager.h"

#	include "Game.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Arrow.h"

#	include "LogEngine.h"

#	include "MousePickerSystem.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"
#	include "Utils.h"
#	include "ScheduleManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Player::Player()
	: m_avatar(0)
	, m_scene(0)
	, m_arrow(0)
	, m_renderCamera2D(0)
	, m_switchScene(false)
	, m_nextScene(0)
	, m_destroyOldScene( false )
	, m_restartScene( false )
	, m_arrowHided( false )
	, m_mousePickerSystem( 0 )
	, m_scheduleManager( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Player::~Player()
	{
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
		m_nextScene = Holder<Game>::hostage()
			->getScene( _name );

		if( m_nextScene == 0 )
		{
			MENGE_LOG_ERROR( "Scene \"%s\" not have in Game"
				, _name.c_str() );
			return;
		}

		if( m_nextScene == m_scene )
		{
			m_restartScene = true;
		}

		m_switchScene = true;
		m_destroyOldScene = _destroyOld;

		scheduleRemoveAll();
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
			return false;
		}

		arrow->setWindow( _contentResolution );


		Camera2D * camera = SceneManager::createNodeT<Camera2D>( "Camera2D" );

		if( camera == 0 )
		{
			return false;
		}
		//mt::vec2f vpSz( 1024, 768 );

		float crx = float( _contentResolution.getWidth() );
		float cry = float( _contentResolution.getHeight() );

		mt::vec2f crv( crx, cry );

		camera->setViewportSize( crv );
		camera->setLocalPosition( crv * 0.5f );

		setRenderCamera2D( camera );

		m_mousePickerSystem = new MousePickerSystem();
		Holder<MousePickerSystem>::keep( m_mousePickerSystem );

		setArrow( arrow );

		m_scheduleManager = new ScheduleManager();

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
			++it)
			{
				GlobalMouseHandler * mouseHandler = *it;
				if( handler = mouseHandler->handleGlobalMouseButtonEvent( _button, _isDown ) )
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
		m_arrow->onMouseMove( _x, _y );

		bool handler = false;

		if( handler == false )
		{
			for( TSetGlobalMouseHandler::iterator
				it = m_setGlobalMouseHandler.begin(),
				it_end = m_setGlobalMouseHandler.end();
			it != it_end;
			++it)
			{
				GlobalMouseHandler * mouseHandler = *it;
				if( handler = mouseHandler->handleGlobalMouseMove( _x, _y, _whell ) )
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
		TSetGlobalMouseHandler::iterator it_find = m_setGlobalMouseHandler.find( _handler );
		if( it_find == m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.insert( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalMouseEventable( GlobalMouseHandler * _handler )
	{
		TSetGlobalMouseHandler::iterator it_find = m_setGlobalMouseHandler.find( _handler );
		if( it_find != m_setGlobalMouseHandler.end() )
		{
			m_setGlobalMouseHandler.erase( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::regGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = m_setGlobalKeyHandler.find( _handler );
		if( it_find == m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.insert( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::unregGlobalKeyEventable( GlobalKeyHandler * _handler )
	{
		TSetGlobalKeyHandler::iterator it_find = m_setGlobalKeyHandler.find( _handler );
		if( it_find != m_setGlobalKeyHandler.end() )
		{
			m_setGlobalKeyHandler.erase( _handler );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::updateChangeScene()
	{
		if( m_restartScene )		// just restart scene
		{
			String name = m_scene->getName();
			m_scene->deactivate();
			m_scene->release();
			if( m_destroyOldScene )
			{
				m_mousePickerSystem->reset();
				Holder<Game>::hostage()->destroyScene( m_scene->getName() );
			}
			m_scene = Holder<Game>::hostage()->getScene( name );
			m_scene->compile();
			m_scene->activate();

			m_restartScene = false;
			m_switchScene = false;
		}
		else if( m_switchScene == true )
		{
			m_renderCamera2D->deactivate();

			m_switchScene = false;

			m_nextScene->compile();
			m_nextScene->activate();


			if( m_scene )
			{
				m_scene->deactivate();
				m_scene->release();
				if( m_destroyOldScene )
				{
					m_mousePickerSystem->reset();
					Holder<Game>::hostage()->destroyScene( m_scene->getName() );
				}
			}
			m_scene = m_nextScene;

			//m_scene->compile();
			//m_scene->activate();
			m_renderCamera2D->activate();
			//Holder<ResourceManager>::hostage()->_dumpResources();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::update( float _timing )
	{
		HotSpot* picker = m_arrow->getCurrentHotSpot();
		m_mousePickerSystem->update( picker );

		const mt::vec2f & arrowPos = m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

		Holder<PhysicEngine2D>::hostage()->onMouseMove( arrowPos );

		updateChangeScene();

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
		//m_mousePickerSystem->clear();
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
			m_scene->render( _debugMask );
		}

		if( m_arrow )
		{
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			renderEngine->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

			renderEngine->beginLayer2D();
			renderEngine->setRenderTarget( "defaultCamera" );
			mt::vec2f pos = m_renderCamera2D->getLocalPosition();
			m_renderCamera2D->setLocalPosition( mt::vec2f( 512.0f, 384.0f ) );
			renderEngine->setViewMatrix( m_renderCamera2D->getViewMatrix() );
			m_arrow->render( _debugMask );

			renderEngine->endLayer2D();
			m_renderCamera2D->setLocalPosition( pos );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Player::onMouseLeave()
	{
		if( m_arrow )
		{
			m_arrow->onMouseLeave();
		}
		if( m_scene )
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
		if( m_scene )
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
	std::size_t Player::timerSchedule( float _timing, PyObject* _func )
	{
		if( ( m_restartScene || m_switchScene ) == true )
		{
			return 0;
		}
		return m_scheduleManager->timerSchedule( _timing, _func );
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
}