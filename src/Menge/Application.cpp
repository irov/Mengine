#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "Amplifier.h"
#	include "ParticleEngine.h"
#	include "ScriptEngine.h"
#	include "PhysicEngine.h"
#	include "PhysicEngine2D.h"

#	include "MousePickerSystem.h"
#	include "ResourceManager.h"

#	include "Game.h"

#	include "Player.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include "SceneManager.h"
#	include "TextField.h"


#define MENGE_DELETE(x) if(x) { delete (x); (x) = NULL; }

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ApplicationInputHandlerProxy
		: public InputHandler
	{
	public:
		ApplicationInputHandlerProxy( Application * _app )
			: m_app( _app )
		{}

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override
		{
			return m_app->onKeyEvent( _key, _char, _isDown );
		}

		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override
		{
			return m_app->onMouseButtonEvent( _button, _isDown );
		}

		bool handleMouseMove( float _x, float _y, int _whell ) override
		{
			return m_app->onMouseMove( _x, _y, _whell );
		}

	private:
		Application * m_app;
	};

	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface )
		: m_interface( _interface )
		, m_commandLine("")
		//, m_resourcePaths("")
		, m_particles( true )
		, m_sound( true )
		, m_debugRender( false )
		, m_phycisTiming(0.f)
		, m_resetTiming( false )
		, m_maxTiming( 100.0f )
		, m_debugInfo( false )
		, m_debugTextField( NULL )
		, m_FPS( 0.0f )
		, m_logEngine( NULL )
		, m_fileEngine( NULL )
		, m_renderEngine( NULL )
		, m_soundEngine( NULL )
		, m_particleEngine( NULL )
		, m_physicEngine2D( NULL )
		, m_physicEngine( NULL )
		, m_xmlEngine( NULL )
	{
		//ASSERT( m_interface );

		Holder<Application>::keep( this );
		m_handler = new ApplicationInputHandlerProxy( this );

	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLogSystem( LogSystemInterface * _interface )
	{
		m_logEngine = new LogEngine( _interface );
		Holder<LogEngine>::keep( m_logEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFileSystem( FileSystemInterface * _interface )
	{
		m_fileEngine = new FileEngine( _interface );
		Holder<FileEngine>::keep( m_fileEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setInputSystem( InputSystemInterface * _interface )
	{
		m_inputEngine = new InputEngine( _interface );
		Holder<InputEngine>::keep( m_inputEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setRenderSystem( RenderSystemInterface * _interface )
	{
		m_renderEngine = new RenderEngine( _interface );
		Holder<RenderEngine>::keep( m_renderEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundSystem( SoundSystemInterface * _interface )
	{
		m_soundEngine = new SoundEngine( _interface );
		Holder<SoundEngine>::keep( m_soundEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticleSystem( ParticleSystemInterface * _interface )
	{
		 m_particleEngine = new ParticleEngine( _interface );
		 Holder<ParticleEngine>::keep( m_particleEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem2D( PhysicSystem2DInterface * _interface )
	{
		m_physicEngine2D = new PhysicEngine2D( _interface );
		Holder<PhysicEngine2D>::keep( m_physicEngine2D );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem( PhysicSystemInterface * _interface )
	{
		m_physicEngine = new PhysicEngine( _interface );
		Holder<PhysicEngine>::keep( m_physicEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPak( const std::string & _pak )
	{
		MENGE_LOG("load pack [%s]...\n", _pak.c_str() );
		m_fileEngine->loadPak( _pak );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame()
	{
		MENGE_LOG("create game file [%s] ...\n", m_gameInfo.c_str() );


		MENGE_LOG("init game ...\n");

		if( m_physicEngine )
		{
			m_physicEngine->init(mt::vec3f(0.f,-1.f,0.f));
		}

		Game* game = Holder<Game>::hostage();


		const std::string & renderDriver = game->getRenderDriverName();

		m_renderEngine->initialize( renderDriver.c_str() );
		/*if( renderDriver == "D3D9" )
		{
#	ifndef _DEBUG
			Holder<RenderEngine>::hostage()->initialize( "RenderSystem_Direct3D9" );
#	else 
			Holder<RenderEngine>::hostage()->initialize( "RenderSystem_Direct3D9_d" );
#	endif
		}
		else if( renderDriver == "OGL" )
		{
#	ifndef _DEBUG
			Holder<RenderEngine>::hostage()->initialize( "RenderSystem_GL" );
#	else 
			Holder<RenderEngine>::hostage()->initialize( "RenderSystem_GL_d" );
#	endif
		}
		else
		{
			MENGE_LOG("Please select render drive 'D3D9' or 'OGL'. Thanks!");
			return false;
		}*/

		m_currentResolution.x = game->getWidth();
		m_currentResolution.y = game->getHeight();
		if( game->getFullscreen() )
		{
			m_currentResolution = m_renderEngine->getBestDisplayResolution( game->getWidth(), game->getHeight(), m_interface->getMonitorAspectRatio() );
		}

		WINDOW_HANDLE winHandle = m_interface->createWindow( game->getTitle().c_str(), m_currentResolution.x, m_currentResolution.y, game->getFullscreen() );
		m_renderEngine->createRenderWindow( m_currentResolution.x, m_currentResolution.y, game->getBits(), game->getFullscreen(), winHandle );
		m_inputEngine->initialize( winHandle );

		if( game->getFullscreen() )
		{
			m_inputEngine->setMouseBounded( true );
		}

		if( game->isContentResolutionFixed() )
		{
			mt::vec2f res = game->getResourceResolution();
			m_renderEngine->setViewportDimensions( res.x, res.y );
		}

		if( game->init() == false )
		{
			return false;
		}

		if( m_debugInfo )	
		{
			m_debugTextField = Menge::SceneManager::createNodeFromXmlT<TextField>( m_debugResourcesPath + "DebugInfo.xml" );
			m_debugTextField->compile();
			m_debugTextField->activate();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Application")
			{				
				XML_PARSE_ELEMENT( this, &Application::loaderApplication );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loaderApplication( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Paks")
			{
				// 
			}

			XML_CASE_ATTRIBUTE_NODE( "Game", "File", m_gameInfo );

			XML_CASE_NODE( "Resource" )
			{
				std::string filename;
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "File", filename );
				}
				m_resourcePaths.push_back( filename );
			}
	
			XML_CASE_ATTRIBUTE_NODE( "DebugResources", "Path", m_debugResourcesPath );
			//XML_CASE_ATTRIBUTE_NODE( "Resources", "File", m_resourcePath );

			/*XML_CASE_NODE("Config")
			{
				XML_PARSE_ELEMENT( this, &Application::loaderConfig );
			}*/
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const std::string & _applicationFile, const std::string& _args )
	{
		if( !m_interface->init( _applicationFile.c_str(), this ) )
		{
			return false;
		}

		int idx = _args.find( "-sound" );
		if( idx >= 0 )
		{
			m_sound = false;
		}
		
		//Временная запись
		//добавил Вова
		//m_sound = false;

		idx = _args.find( "-particles" );
		if( idx >= 0 )
		{
			m_particles = false;
		}

		idx = _args.find( "-debugRender" );
		if( idx >= 0 )
		{
			m_debugRender = true;
		}

		idx = _args.find( "-debuginfo" );
		if( idx >= 0 )
		{
			m_debugInfo = true;
		}

		// Initializing XML-engine
		m_xmlEngine = new XmlEngine;
		Holder<XmlEngine>::keep( m_xmlEngine );

		// Reading resources
		if( m_xmlEngine->parseXmlFileM( _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG("parse application xml failed '%s'\n"
				, _applicationFile.c_str()
				);
		}

		// prepare file system
		m_fileEngine->changeDir( "../" );

		// prepare resources
		Holder<ScriptEngine>::keep( new ScriptEngine );
		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();
		MENGE_LOG("init scriptEngine ...\n");
		scriptEngine->init();

		Game* game = new Game;
		Holder<Game>::keep( game );

		if( m_xmlEngine->parseXmlFileM( m_gameInfo, game, &Game::loader ) == false )
		{
			MENGE_LOG("Invalid game file [%s] ...\n", m_gameInfo.c_str() );
			return false;
		}

		//prepareResources();
		for( TStringVector::iterator it = m_resourcePaths.begin(), 
				it_end = m_resourcePaths.end(); it != it_end; it++ )
		{
			game->readResourceFile( *it );
		}
		
		const std::string& physicSystemName = game->getPhysicSystemName();
		if( physicSystemName != "None" )
		{
			SystemDLLInterface* dll = m_interface->loadSystemDLL( physicSystemName.c_str() );
			setPhysicSystem( dll->getInterface<PhysicSystemInterface>() );
		}

		m_inputEngine->regHandle( m_handler );

		Holder<ResourceManager>::keep( new ResourceManager );

		if( createGame() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{
		if( Holder<Game>::empty() == false )
		{
			Holder<Game>::hostage()->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		return Holder<Game>::hostage()->handleKeyEvent( _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( int _button, bool _isDown )
	{
		return Holder<Game>::hostage()->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( float _x, float _y, int _whell )
	{
		InputEngine* inpEng = Holder<InputEngine>::hostage();
		float oldx = 0.f;
		float oldy = 0.f;
		if( !inpEng->getMouseBounded() )
		{ 
			oldx = inpEng->getMouseX();
			oldy = inpEng->getMouseY();
			inpEng->setMousePos( _x, _y );
		}
		return Holder<Game>::hostage()->handleMouseMove( _x - oldx, _y - oldy, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseLeave()
	{
		Holder<Game>::hostage()->handleMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseEnter()
	{
		Holder<Game>::hostage()->handleMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		//m_quit = true; 
		m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::frameStarted()
	{
		//Holder<RenderEngine>::hostage()->frameStarted();
	}
	//////////////////////////////////////////////////////////////////////////
	/*const std::string& Application::getResourcePath() const
	{
		return m_resourcePath;
	}*/
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticlesEnabled( bool _enabled )
	{
		m_particles = _enabled;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getParticlesEnabled() const
	{
		return m_particles;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundEnabled( bool _enabled )
	{
		m_sound = _enabled;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getSoundEnabled() const
	{
		return m_sound;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::usePhysic() const
	{
		if( m_physicEngine )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onFocus( bool _focus )
	{
		static float volume = 1.0f;
		static float avolume = 1.0f;
		if( !_focus )
		{
			volume = Holder<SoundEngine>::hostage()->getCommonVolume();
			avolume = Holder<Amplifier>::hostage()->getVolume();
			m_soundEngine->setCommonVolume( 0.0f );
			Holder<Amplifier>::hostage()->setVolume( 0.0f );
		}
		else
		{
			m_resetTiming = true;
			m_soundEngine->setCommonVolume( volume );
			Holder<Amplifier>::hostage()->setVolume( avolume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::minimizeWindow()
	{
		m_interface->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::run()
	{
		m_interface->run();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onUpdate( float _timing )
	{
		//MENGE_LOG("WinApplication: %.2f\n", m_interface->getDeltaTime() );

		if( m_debugInfo )
		{
			static int d = 0;
			d++;
			if( !(d % 10) )
			{
				m_FPS = 1000.0f / _timing;
				//printf("fps: %.2f\n", m_FPS );
			}
		}

		if( _timing > m_maxTiming )
		{
			_timing = m_maxTiming;
		}

		if( m_resetTiming )
		{
			_timing = 0.0f;
			m_resetTiming = false;
		}

		Holder<MousePickerSystem>::hostage()->clear();

		if( m_physicEngine )
		{
			m_physicEngine->update( 1.0f/30.0f );// for test physic!
		}

		if( m_physicEngine2D )
		{
			m_physicEngine2D->update( _timing );
		}
		//MENGE_LOG("Physic2DUpdate: %.2f\n", m_interface->getDeltaTime() );


		Holder<Game>::hostage()->update( _timing );
		//MENGE_LOG("GameUpdate: %.2f\n", m_interface->getDeltaTime() );
		m_inputEngine->update();
		//MENGE_LOG("InputUpdate: %.2f\n", m_interface->getDeltaTime() );
		Holder<MousePickerSystem>::hostage()->update();
		//MENGE_LOG("MousePickerUpdate: %.2f\n", m_interface->getDeltaTime() );
		m_soundEngine->update( _timing );
		//MENGE_LOG("SoundUpdate: %.2f\n", m_interface->getDeltaTime() );

		Holder<Game>::hostage()->render();
		//MENGE_LOG("GameRender: %.2f\n", m_interface->getDeltaTime() );
		//Holder<Game>::hostage()->debugRender();
		
		if( m_debugInfo )
		{
			sprintf( m_debugText, "FPS:%.2f\n", m_FPS );
			m_debugTextField->setText( m_debugText );
			m_debugTextField->render();
		}
		if(m_debugRender)
		{
			Holder<Game>::hostage()->debugRender();
		}

		m_renderEngine->render();
		//MENGE_LOG("RenderTime: %.2f\n", m_interface->getDeltaTime() );

	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onClose()
	{
		quit();
		//m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onDestroy()
	{
		delete m_handler;

		if( m_debugTextField )
		{
			m_debugTextField->release();
			delete m_debugTextField;
		}

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();

		Holder<PhysicEngine>::empty();
		Holder<PhysicEngine2D>::empty();
		Holder<ParticleEngine>::empty();
		Holder<RenderEngine>::empty();
		Holder<FileEngine>::empty();
		Holder<InputEngine>::empty();
		Holder<SoundEngine>::empty();
		Holder<XmlEngine>::empty();

		MENGE_DELETE( m_physicEngine );
		MENGE_DELETE( m_physicEngine2D );
		MENGE_DELETE( m_particleEngine );
		MENGE_DELETE( m_renderEngine );
		MENGE_DELETE( m_fileEngine );
		MENGE_DELETE( m_inputEngine );
		MENGE_DELETE( m_soundEngine );
		MENGE_DELETE( m_xmlEngine );
		
		Holder<ScriptEngine>::destroy();

		if( m_physicEngine != 0 )
		{
			//??
			//m_interface->unloadSystemDLL(m_physicEngine->);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onWindowMovedOrResized()
	{
		m_renderEngine->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Application::getCurrentResolution() const
	{
		return m_currentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::notifyWindowModeChanged( float _width, float _height, bool _fullscreen )
	{
		m_interface->notifyWindowModeChanged( _width, _height, _fullscreen );
	}
}
