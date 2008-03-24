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
		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override
		{
			return m_app->handleKeyEvent( _key, _char, _isDown );
		}

		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override
		{
			return m_app->onMouseButtonEvent( _button, _isDown );
		}

		bool handleMouseMove( int _x, int _y, int _whell ) override
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
		, m_physicEngine2D( NULL )
		, m_physicEngine( NULL )
		, m_phycisTiming(0.f)
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
		new LogEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFileSystem( FileSystemInterface * _interface )
	{
		new FileEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setInputSystem( InputSystemInterface * _interface )
	{
		new InputEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setRenderSystem( RenderSystemInterface * _interface )
	{
		new RenderEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundSystem( SoundSystemInterface * _interface )
	{
		new SoundEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticleSystem( ParticleSystemInterface * _interface )
	{
		 new ParticleEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem2D( PhysicSystem2DInterface * _interface )
	{
		m_physicEngine2D = new PhysicEngine2D( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem( PhysicSystemInterface * _interface )
	{
		m_physicEngine = new PhysicEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPak( const std::string & _pak )
	{
		MENGE_LOG("load pack [%s]...\n", _pak.c_str() );
		Holder<FileEngine>::hostage()->loadPak( _pak );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame()
	{
		MENGE_LOG("create game file [%s] ...\n", m_gameInfo.c_str() );


		MENGE_LOG("init game ...\n");

		if( m_physicEngine )
		{
			m_physicEngine->init(mt::vec3f(0,-1.0,0));
		}

		Game* game = Holder<Game>::hostage();

		Holder<RenderEngine>::hostage()->initialize( game->getRenderDriverName() );
		m_currentResolution.x = game->getWidth();
		m_currentResolution.y = game->getHeight();
		if( game->getFullscreen() )
		{
			m_currentResolution = Holder<RenderEngine>::hostage()->getBestDisplayResolution( game->getWidth(), game->getHeight(), m_interface->getMonitorAspectRatio() );
		}

		WINDOW_HANDLE winHandle = m_interface->createWindow( game->getTitle().c_str(), m_currentResolution.x, m_currentResolution.y, game->getFullscreen() );
		Holder<RenderEngine>::hostage()->createRenderWindow( m_currentResolution.x, m_currentResolution.y, game->getBits(), game->getFullscreen(), winHandle );
		Holder<InputEngine>::hostage()->initialize( winHandle );

		if( game->isContentResolutionFixed() )
		{
			mt::vec2f res = game->getResourceResolution();
			Holder<RenderEngine>::hostage()->setViewportDimensions( res.x, res.y );
		}

		if( game->init() == false )
		{
			return false;
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

		// Initializing XML-engine
		Holder<XmlEngine>::keep( new XmlEngine );

		// Reading resources
		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG("parse application xml failed '%s'\n"
				, _applicationFile.c_str()
				);
		}

		// prepare file system
		Holder<FileEngine>::hostage()->changeDir( "../" );

		// prepare resources
		Holder<ScriptEngine>::keep( new ScriptEngine );
		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();
		MENGE_LOG("init scriptEngine ...\n");
		scriptEngine->init();

		Game* game = new Game;
		Holder<Game>::keep( game );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( m_gameInfo, game, &Game::loader ) == false )
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


		InputEngine * inputEng = Holder<InputEngine>::hostage();

		inputEng->regHandle( m_handler );

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
	bool Application::handleKeyEvent( size_t _key, size_t _char, bool _isDown )
	{
		return Holder<Game>::hostage()->handleKeyEvent( _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( int _button, bool _isDown )
	{
		return Holder<Game>::hostage()->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( int _x, int _y, int _whell )
	{
		InputEngine* inpEng = Holder<InputEngine>::hostage();
		int oldx = 0;
		int oldy = 0;
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
			Holder<SoundEngine>::hostage()->setCommonVolume( 0.0f );
			Holder<Amplifier>::hostage()->setVolume( 0.0f );
		}
		else
		{
			Holder<SoundEngine>::hostage()->setCommonVolume( volume );
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
		Holder<MousePickerSystem>::hostage()->clear();

		if( m_physicEngine )
		{
			m_physicEngine->update( 1.0f/30.0f );// for test physic!
		}

		if( m_physicEngine2D )
		{
			float timeStep = 1.f / 60.f;
			m_phycisTiming += _timing;
			while( m_phycisTiming >= timeStep * 1000.f )
			{
				m_physicEngine2D->update( timeStep, 10.f );
				m_phycisTiming -= timeStep * 1000.f;
			}
		}

		Holder<Game>::hostage()->update( _timing );
		Holder<InputEngine>::hostage()->update();
		Holder<MousePickerSystem>::hostage()->update();
		Holder<SoundEngine>::hostage()->update( _timing );

		Holder<Game>::hostage()->render();
		Holder<Game>::hostage()->debugRender();
		Holder<RenderEngine>::hostage()->render();
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

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();

		if ( m_physicEngine )
		{
			Holder<PhysicEngine>::destroy();
		}
		if( m_physicEngine2D )
		{
			Holder<PhysicEngine2D>::destroy();
		}

		Holder<ParticleEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<ScriptEngine>::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onWindowMovedOrResized()
	{
		Holder<RenderEngine>::hostage()->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Application::getCurrentResolution() const
	{
		return m_currentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::notifyWindowModeChanged( unsigned int _width, unsigned int _height, bool _fullscreen )
	{
		m_interface->notifyWindowModeChanged( _width, _height, _fullscreen );
	}
}
