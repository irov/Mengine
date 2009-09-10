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
#	include "LightSystem.h"

#	include "Game.h"

#	include "Interface/PhysicSystem2DInterface.h"
#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include "SceneManager.h"

#	include "TextManager.h"
#	include "TextField.h"

#	include "ThreadManager.h"
#	include "TaskManager.h"

#	include "FactoryDeclare.h"

#	include "DecoderManager.h"
#	include "EncoderManager.h"
#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"

#	include "Player.h"
#	include "Scene.h"

#	include "ImageDecoderMNE.h"
#	include "ImageDecoderPNG.h"
#	include "ImageEncoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "VideoDecoderOGGTheora.h"
#	include "SoundDecoderOGGVorbis.h"

#	include <locale.h>
#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "VersionInfo.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface
								, const String& _userPath
								, const String& _scriptInitParams
								, OutputStreamInterface* _platformLogger )
		: m_interface( _interface )
		, m_platformLogger( _platformLogger )
		, m_scriptInitParams( _scriptInitParams )
		, m_particles( true )
		, m_sound( true )
		, m_debugMask( 0 )
		, m_phycisTiming(0.f)
		, m_resetTiming( false )
		, m_maxTiming( 100.0f )
		, m_logEngine( 0 )
		, m_fileEngine( 0 )
		, m_renderEngine( 0 )
		, m_soundEngine( 0 )
		, m_particleEngine( 0 )
		, m_physicEngine2D( 0 )
		, m_physicEngine( 0 )
		, m_xmlEngine( 0 )
		, m_mouseBounded( false )
		, m_game( 0 )
		, m_focus( true )
		, m_update( true )
		, m_enableDebug( false )
		, m_userPath( _userPath )
		, m_altDown( 0 )
		, m_gameInfo("")
		, m_baseDir("")
		, m_console(NULL)
		, m_scriptEngine(NULL)
		, m_fileLog( NULL )
		, m_threadSystem( NULL )
		, m_threadManager( NULL )
		, m_taskManager( NULL )
		, m_alreadyRunningPolicy( 0 )
		, m_allowFullscreenSwitchShortcut( true )
		, m_resourceManager( NULL )
		, m_alphaChannelManager( NULL )
		, m_decoderManager( NULL )
		, m_encoderManager( NULL )
		, m_textManager( NULL )
	{
		m_logEngine = new LogEngine();
		if( m_logEngine->initialize() == false )
		{
			// shit
		}
		if( m_platformLogger != NULL )
		{
			m_logEngine->registerLogger( m_platformLogger );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		finalize();
		if( m_platformLogger != NULL )
		{
			m_logEngine->unregisterLogger( m_platformLogger );
		}
		delete m_logEngine;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFileSystem( FileSystemInterface * _interface )
	{
		m_fileEngine = new FileEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setInputSystem( InputSystemInterface * _interface )
	{
		m_inputEngine = new InputEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setRenderSystem( RenderSystemInterface * _interface )
	{
		m_renderEngine = new RenderEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundSystem( SoundSystemInterface * _interface )
	{
		m_soundEngine = new SoundEngine( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticleSystem( ParticleSystemInterface * _interface )
	{
		 m_particleEngine = new ParticleEngine( _interface );
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
	const String& Application::getPathGameFile() const
	{
		return m_gameInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Application::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadGame( bool _loadPersonality )
	{
		m_game = new Game();

		MENGE_LOG( "Create game file \"%s\""
			, m_gameInfo.c_str() );

		//m_fileEngine->loadPak( m_gamePack );
		if( m_fileEngine->mountFileSystem( m_gamePackName, m_gamePackPath, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: (Application::loadGame) failed to mount GamePak \"%s\"",
				m_gamePackPath.c_str() );
			return false;
		}

		if( m_xmlEngine->parseXmlFileM( m_gamePackName, m_gameInfo, m_game, &Game::loader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid game file \"%s\""
				, m_gameInfo.c_str() );
			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		if( m_baseDir.empty() )	// current dir
		{
			m_baseDir = ".";
		}

		m_game->setBaseDir( m_baseDir );
		if( m_languagePackOverride.empty() == false )
		{
			m_game->setLanguagePack( m_languagePackOverride );
		}
		m_game->loadConfigPaks();
		//m_game->registerResources( m_baseDir );
		
		String title = m_game->getTitle();
		bool fullscreen = m_game->getFullscreen();
		m_renderEngine->setFullscreenMode( fullscreen );

		if( _loadPersonality )
		{
			if( m_game->loadPersonality() == false )
			{
				return false;
			}
		}

		//m_fileEngine->loadPak( "Game.zip" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle )
	{
		const String & title = m_game->getTitle();

		const Resolution & resourceResolution = m_game->getResourceResolution();

		m_renderEngine->setContentResolution( resourceResolution );

		bool isFullscreen = m_renderEngine->getFullscreenMode();

		if( isFullscreen == true )
		{
			//float aspect = m_desktopResolution.getRatio();
			//m_currentResolution = m_renderEngine->getBestDisplayResolution( m_desktopResolution, aspect );
			m_currentResolution = m_desktopResolution;
		}
		else
		{
			m_currentResolution = m_game->getResolution();
		}

		int bits = m_game->getBits();
		int FSAAType = m_game->getFSAAType();
		int FSAAQuality = m_game->getFSAAQuality();
		bool vsync = m_game->getVSync();
		bool textureFiltering = m_game->getTextureFiltering();

		bool res = m_renderEngine->createRenderWindow( m_currentResolution, bits, isFullscreen, _renderWindowHandle,
											vsync, FSAAType, FSAAQuality );
		if( res == false )
		{
			showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}
		m_renderEngine->enableTextureFiltering( textureFiltering );



		MENGE_LOG( "Initializing Input Engine..." );
		bool result = m_inputEngine->initialize( _inputWindowHandle );
		if( result == true )
		{
			MENGE_LOG( "Input Engine successfully!" );
		}
		else
		{
			MENGE_LOG( "Input Engine initialization failed!" );
		}
		m_inputEngine->setResolution( resourceResolution[0], resourceResolution[1] );
		
		if( isFullscreen == true )
		{
			//setMouseBounded( true );
			m_inputEngine->setMouseBounded( true );
			m_interface->setHandleMouse( false );
		}

		//Holder<Console>::keep( new Console() );
		//Holder<Console>::hostage()->inititalize( m_logSystem );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initGame()
	{
		if( m_game->init( m_scriptInitParams ) == false )
		{
			return false;
		}

		m_game->update( 0.0f );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Application" )
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
			XML_CASE_ATTRIBUTE_NODE( "BaseDir", "Value", m_baseDir );
			XML_CASE_NODE( "GamePack" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", m_gamePackName );
					XML_CASE_ATTRIBUTE( "Path", m_gamePackPath );
					XML_CASE_ATTRIBUTE( "Description", m_gameInfo );
				}
			}
			XML_CASE_ATTRIBUTE_NODE( "AlreadyRunningPolicy", "Value", m_alreadyRunningPolicy );
			XML_CASE_ATTRIBUTE_NODE( "AllowFullscreenSwitchShortcut", "Value", m_allowFullscreenSwitchShortcut );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments_( const String& _arguments )
	{
		String::size_type idx = _arguments.find( "-sound" );
		if( idx != String::npos )
		{
			m_sound = false;
		}

		idx = _arguments.find( "-particles" );
		if( idx != String::npos )
		{
			m_particles = false;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Application::exec( const Menge::String & _text )
	{
		m_scriptEngine->exec( _text );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::registerConsole( ConsoleInterface * _console )
	{
		m_console = _console;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderSystemInterface * Application::getRenderSystem() const
	{
		return m_renderSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const String& _applicationFile, const String & _args, bool _loadPersonality )
	{

		parseArguments_( _args );

		//String loc = setlocale( LC_CTYPE, NULL ); // default (OS) locale

		m_resourceManager = new ResourceManager();
		m_resourceManager->initialize();
		//Holder<ResourceManager>::keep( resourceMgr );

		m_alphaChannelManager = new AlphaChannelManager();
		m_textManager = new TextManager();
		
		//Holder<Console>::keep( new Console() );
		parseArguments_( _args );

		MENGE_LOG( "Initializing Thread System..." );
		initInterfaceSystem( &m_threadSystem );
		m_threadManager = new ThreadManager( m_threadSystem );

		MENGE_LOG( "Inititalizing File System..." );
		initInterfaceSystem( &m_fileSystem );
		this->setFileSystem( m_fileSystem );

		// mount root
		if( m_fileEngine->mountFileSystem( "", "./", false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount root directory" );
			return false;
		}

		// mount user directory
		if( m_fileEngine->mountFileSystem( "user", m_userPath, true ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount user directory" );
		}

		String logFilename = "Game";

		if( m_enableDebug == true )
		{
			std::stringstream dateStream;
			std::time_t ctTime; 
			std::time(&ctTime);
			std::tm* sTime = std::localtime( &ctTime );
			dateStream << 1900 + sTime->tm_year << "_" << std::setw(2) << std::setfill('0') <<
				(sTime->tm_mon+1) << "_" << std::setw(2) << std::setfill('0') << sTime->tm_mday << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_hour << "_" 
				<< std::setw(2) << std::setfill('0') << sTime->tm_min << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_sec;

			String dateString = dateStream.str();
			logFilename += "_";
			logFilename += dateString;
		}
		logFilename += ".log";

		m_fileLog = m_fileEngine->openFileOutput( "user", logFilename );
		if( m_fileLog != NULL )
		{
			m_logEngine->registerLogger( m_fileLog );
			m_logEngine->logMessage( "Starting log to Menge.log\n" );
		}

		MENGE_LOG( "Initializing Input System..." );
		initInterfaceSystem( &m_inputSystem );
		this->setInputSystem( m_inputSystem );

#	if	MENGE_PARTICLES	== (1)
		MENGE_LOG( "Initializing Particle System..." );
		initInterfaceSystem( &m_particleSystem );
		this->setParticleSystem( m_particleSystem );
#	endif
		
		MENGE_LOG( "Inititalizing Physics2D System..." );
		initInterfaceSystem( &m_physicSystem2D );
		this->setPhysicSystem2D( m_physicSystem2D );
		
		MENGE_LOG( "Initializing Render System..." );
		initInterfaceSystem( &m_renderSystem );
		this->setRenderSystem( m_renderSystem );
		
		MENGE_LOG( "Initializing Sound System..." );
		initInterfaceSystem( &m_soundSystem );
		this->setSoundSystem( m_soundSystem );

		bool res = m_soundEngine->initialize();

		if( m_sound == false )
		{
			m_soundEngine->mute( true );
		}

		if( res == false )
		{
			m_sound = false;
		}

		res = m_renderEngine->initialize( 4000 );

		if( res == false )
		{
			showMessageBox( "Failed to initialize Render System", "Crititcal Error", 0 );
			return false;
		}

		m_taskManager = new TaskManager();

		initializeSceneManager_();

		MENGE_LOG( "Initializing Xml Engine..." );
		m_xmlEngine = new XmlEngine();

		if( m_xmlEngine->parseXmlFileM( "", _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG_ERROR( "parse application xml failed \"%s\""
				, _applicationFile.c_str() );
			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		m_fileEngine->setBaseDir( m_baseDir );

		MENGE_LOG( "Initializing Script Engine..." );

		m_scriptEngine = new ScriptEngine();
		m_scriptEngine->init();

		MENGE_LOG( "Inititalizing Codecs..." );

		m_decoderManager = new DecoderManager();
		m_encoderManager = new EncoderManager();

		//Holder<Console>::hostage()->inititalize( m_logSystem );
		// Decoders
		//MENGE_REGISTER_DECODER( "Image", ImageDecoderMNE, "mne" );

		DECODER_FACTORY( "Image", ImageDecoderPNG, "png" );
		DECODER_FACTORY( "Image", ImageDecoderJPEG, "jpeg" );
		DECODER_FACTORY( "Image", ImageDecoderJPEG, "jpg" );
		DECODER_FACTORY( "Image", ImageDecoderMNE, "mne" );

		VideoDecoderOGGTheora::createCoefTables_();

		DECODER_FACTORY( "Video", VideoDecoderOGGTheora, "ogg" );
		DECODER_FACTORY( "Video", VideoDecoderOGGTheora, "ogv" );
		DECODER_FACTORY( "Sound", SoundDecoderOGGVorbis, "ogg" );
		DECODER_FACTORY( "Sound", SoundDecoderOGGVorbis, "ogv" );
		// Encoders
		ENCODER_FACTORY( "Image", ImageEncoderPNG, "png" );
		
		//loadPlugins_("");

		loadGame( _loadPersonality );

		//if( m_console != NULL )
		//{
		//	m_console->inititalize( m_logSystem );
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		unsigned int chr = _char;
		/*if( _char >= 128 )
		{
			String ansiChar( 1, (char)_char );
			String utf8char = m_interface->ansiToUtf8( ansiChar );
			chr = 0;
			for( int i = utf8char.length()-1; i >= 0; i-- )
			{
				unsigned char c = utf8char[i];
				unsigned int d = 0;
				d = c << (i*8);
				chr |= d;
			}
		}*/

		if( m_console != NULL )
		{
			m_console->proccessInput( _key, _char, _isDown );
		}

		if( _key == 0x38 || _key == 0xB8 ) // ALT
		{
			if( _isDown )
			{
				m_altDown += 1;
			}
			else
			{
				m_altDown -= 1;
			}
		}

		if( m_allowFullscreenSwitchShortcut == true
			&& ( _key == 0x1C || _key == 0x9C ) && _isDown && ( m_altDown > 0 ) ) // Enter
		{
			if( m_renderEngine != NULL )
			{
				setFullscreenMode( !getFullscreenMode() );
			}
		}

#	ifndef MENGE_MASTER_RELEASE
		if( _key == 88 && _isDown && m_enableDebug ) // F12
		{
			if( ( m_debugMask & MENGE_DEBUG_HOTSPOTS ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_HOTSPOTS;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_HOTSPOTS;
			}
		}

		if( _key == 68 && _isDown && m_enableDebug) // F10
		{
			if( ( m_debugMask & MENGE_DEBUG_NODES ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_NODES;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_NODES;
			}
		}

		if( _key == 67 && _isDown && m_enableDebug) // F9
		{
			if( ( m_debugMask & MENGE_DEBUG_PHYSICS ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_PHYSICS;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_PHYSICS;
			}
		}

		if( _key == 66 && _isDown && m_enableDebug) // F8
		{
			if( ( m_debugMask & MENGE_DEBUG_TILEPOLYGON ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_TILEPOLYGON;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_TILEPOLYGON;
			}
		}

		if( _key == 0x3F && _isDown && m_enableDebug ) // F5
		{
			m_resourceManager->dumpResources("Application");
		}

		if( _key == 87 && _isDown && m_enableDebug ) // F11
		{
			Player::hostage()
				->toggleDebugText();
		}

#	endif

		return m_game->handleKeyEvent( _key, chr, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( int _button, bool _isDown )
	{
		return m_game->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( float _dx, float _dy, int _whell )
	{
		/*float oldx = 0.f;
		float oldy = 0.f;

		if( !m_inputEngine->getMouseBounded() )
		{ 
			oldx = m_inputEngine->getMouseX();
			oldy = m_inputEngine->getMouseY();
			m_inputEngine->setMousePos( _x, _y );
		}*/

		return m_game->handleMouseMove( _dx, _dy, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseLeave()
	{
		m_game->handleMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseEnter()
	{
		m_game->handleMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		m_interface->stop();
		m_renderEngine->onWindowClose();
	}
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
		//return;
		if( m_focus == _focus ) return;
		m_focus = _focus;

		m_soundEngine->onFocus( _focus );

		if( m_game != NULL )
		{
			m_game->onFocus( m_focus );
		}
		/*if( m_focus == true )
		{
			m_update = true;
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::minimizeWindow()
	{
		m_interface->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onUpdate( float _timing )
	{
		if( !m_update && !m_focus ) 
		{
			m_taskManager->update();
			return;
		}

		if( m_renderEngine->beginScene() == true )
		{
			m_game->render( m_debugMask );

			if( m_console != NULL )
			{
				m_console->render();
			}
			//Holder<Console>::hostage()->render();

			m_renderEngine->endScene();
		}
		//m_renderEngine->swapBuffers();

		m_inputEngine->update();

		float timing = _timing;

		if( _timing > m_maxTiming )
		{
			timing = m_maxTiming;
		}

		/*if( m_physicEngine )
		{
			m_physicEngine->update( 1.0f/30.0f );
		}*/

     	/*if( m_physicEngine2D )
		{
			m_physicEngine2D->update( timing );
		}*/
		m_taskManager->update();

		m_game->update( timing );
		
		m_soundEngine->update( _timing );
		
		m_renderEngine->swapBuffers();

		if( !m_focus && m_update )
		{
			m_update = false;
		}
		else if( m_focus && !m_update )
		{
			m_update = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onClose()
	{
		bool needQuit = true;
		if( m_game != NULL )
		{
			needQuit = m_game->onClose();
		}
		if( needQuit == true )
		{
			quit();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{
		unloadPlugins_();

		delete m_game;

		//Holder<Console>::destroy();
		delete m_textManager;
		delete m_sceneManager;

		delete m_taskManager;

		delete m_alphaChannelManager;
		delete m_resourceManager;
		delete m_scriptEngine;

		delete m_physicEngine;
		delete m_physicEngine2D;
		delete m_particleEngine;
		delete m_renderEngine;
		delete m_inputEngine;
		delete m_soundEngine;
		
		delete m_decoderManager;
		delete m_encoderManager;

		delete m_xmlEngine;


		releaseInterfaceSystem( m_soundSystem );
		releaseInterfaceSystem( m_renderSystem );
		releaseInterfaceSystem( m_physicSystem2D );
//#	if	MENGE_PARTICLES	== (1)
		releaseInterfaceSystem( m_particleSystem );
//#	endif
		releaseInterfaceSystem( m_inputSystem );

		if( m_fileLog != NULL )
		{
			m_logEngine->unregisterLogger( m_fileLog );
			m_fileEngine->closeFileOutput( m_fileLog );
			m_fileLog = NULL;
		}

		delete m_fileEngine;
		releaseInterfaceSystem( m_fileSystem );

		delete m_threadManager;
		releaseInterfaceSystem( m_threadSystem );

		//		releaseInterfaceSystem( m_profilerSystem );
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getCurrentResolution() const
	{
		return m_currentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_interface->notifyWindowModeChanged( _width, _height, _fullscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMouseBounded( bool _bounded )
	{
		if( m_mouseBounded != _bounded )
		{
			if( !m_renderEngine->getFullscreenMode() )
			{
				if( _bounded == false )
				{
					m_interface->setCursorPosition( m_inputEngine->getMouseX(), m_inputEngine->getMouseY() );
				}
				m_interface->setHandleMouse( !_bounded );
				m_inputEngine->setMouseBounded( _bounded );
			}
			m_mouseBounded = _bounded;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFullscreenMode( bool _fullscreen )
	{
		bool fs = m_renderEngine->getFullscreenMode();
		m_renderEngine->setFullscreenMode( _fullscreen );
		if( fs != _fullscreen )
		{
			m_game->onFullscreen( _fullscreen );
		}

		if( !m_mouseBounded && m_renderEngine->isWindowCreated() )
		{
			if( !_fullscreen )
			{
				m_interface->setCursorPosition( m_inputEngine->getMouseX(), m_inputEngine->getMouseY() );
				m_interface->setHandleMouse( true );
				m_inputEngine->setMouseBounded( false );
				m_currentResolution = m_game->getResolution();
			}
			else
			{
				m_interface->setHandleMouse( false );
				m_inputEngine->setMouseBounded( true );
				m_currentResolution = m_desktopResolution;
			}
			m_game->handleMouseEnter();	
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFullscreenMode()
	{
		return m_renderEngine->getFullscreenMode();
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getDesktopResolution() const
	{
		return m_desktopResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Application::getDebugMask() const
	{
		return m_debugMask;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		m_interface->showMessageBox( _message, _header, _style );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onPaint()
	{
		if( !m_focus && m_renderEngine && m_game )
		{
			m_renderEngine->beginScene();
			m_game->render( m_debugMask );
			m_renderEngine->endScene();
			m_renderEngine->swapBuffers();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	String Application::getProjectTitle() const
	{
		return m_game->getTitle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDesktopResolution( const Resolution& _resolution )
	{
		m_desktopResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String Application::ansiToUtf8( const String& _ansi )
	{
		return m_interface->ansiToUtf8( _ansi );
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String Application::utf8ToAnsi( const String& _utf8 )
	{
		return m_interface->utf8ToAnsi( _utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getHasWindowPanel() const
	{
		return m_game->getHasWindowPanel();
	}
	//////////////////////////////////////////////////////////////////////////
	const char* Application::getVersionInfo()
	{
		return s_versionInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution& Application::getResolution() const
	{
		return m_game->getResolution();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::enableDebug( bool _enable )
	{
		m_enableDebug = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPlugins_( const String& _pluginsFolder )
	{
		loadPlugin_("DebugConsole.dll");
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPlugin_( const String& _pluginName )
	{
        DynamicLibraryInterface * lib = m_interface->load( _pluginName );

		m_plugins.push_back( lib );

		TFunctionPtr function =
			lib->getSymbol("dllStartPlugin");

		if ( function )
		{
			function( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::unloadPlugins_()
    {
		for ( TPluginVec::reverse_iterator it = m_plugins.rbegin(); it != m_plugins.rend(); ++it )
		{
			TFunctionPtr function =
				(*it)->getSymbol("dllShutdownPlugin");

			if ( function )
			{
				function( this );
			}

			m_interface->unload( *it );			
		}

        m_plugins.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMousePosition( int _x, int _y )
	{
		if( m_inputEngine )
		{
			const Resolution& contentRes = m_game->getResourceResolution();
			float fx = static_cast<float>( contentRes[0] ) / static_cast<float>( m_currentResolution[0] ) * _x;
			float fy = static_cast<float>( contentRes[1] ) / static_cast<float>( m_currentResolution[1] ) * _y;
			m_inputEngine->setMousePos( fx, fy );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::injectMouseMove( int _dx, int _dy, int _dz )
	{
		if( m_inputEngine )
		{
			m_inputEngine->handleMouseMove( _dx, _dy, _dz );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLanguagePack( const String& _packName )
	{
		m_languagePackOverride = _packName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getVSync() const
	{
		if( m_game != NULL )
		{
			return m_game->getVSync();
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight )
	{
		m_maxClientAreaSize[0] = _maxWidth;
		m_maxClientAreaSize[1] = _maxHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution& Application::getMaxClientAreaSize() const
	{
		return m_maxClientAreaSize;
	}
	//////////////////////////////////////////////////////////////////////////
	int Application::getAlreadyRunningPolicy() const
	{
		return m_alreadyRunningPolicy;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getAllowFullscreenSwitchShortcut() const
	{
		return m_allowFullscreenSwitchShortcut;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::initializeSceneManager_()
	{
		m_sceneManager = new SceneManager();
		m_sceneManager->initialize();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLoggingLevel( EMessageLevel _level )
	{
		m_logEngine->setVerboseLevel( _level );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::logMessage( const String& _message, EMessageLevel _level )
	{
		m_logEngine->logMessage( _message, _level );
	}
	//////////////////////////////////////////////////////////////////////////
}

