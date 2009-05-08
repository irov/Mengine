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

#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include "SceneManager.h"

#	include "TextManager.h"
#	include "TextField.h"

#	include "Camera2D.h"

#	include "NodeFactory.h"
#	include "ThreadManager.h"
#	include "TaskManager.h"

#	include "Entity.h"
#	include "Animation.h"
#	include "Arrow.h"
#	include "Emitter.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Point.h"
#	include "RigidBody2D.h"
#	include "TilePolygon.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Video.h"
#	include "Layer2DLoop.h"
#	include "Layer2DAccumulator.h"
#	include "CapsuleController.h"
#	include "Layer2D.h"
#	include "Layer3D.h"
#	include "LayerScene.h"
#	include "RenderMesh.h"
#	include "Camera3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"

#	include "ResourceAnimation.h"
#	include "ResourceCapsuleController.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceFont.h"
#	include "ResourceTilePolygon.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceImageCell.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageSet.h"
#	include "ResourceVideo.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ResourcePhysicBoxGeometry.h"
#	include "ResourcePhysicConcaveGeometry.h"
#	include "ResourcePhysicConvexGeometry.h"
#	include "ResourcePlaylist.h"
#	include "ResourceSound.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "ResourceMeshMS3D.h"
#	include "ResourceMeshNoise.h"
#	include "ResourceMaterial.h"
#	include "ResourceWindow.h"
#	include "ResourceHotspotImage.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Codec.h"
#	include "ImageDecoderMNE.h"
#	include "ImageDecoderPNG.h"
#	include "ImageEncoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "VideoDecoderOGGTheora.h"
#	include "SoundDecoderOGGVorbis.h"

#	include <locale.h>

#	include "VersionInfo.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface, const String& _userPath, bool _userLocal,
		const String& _scriptInitParams )
		: m_interface( _interface )
		, m_scriptInitParams( _scriptInitParams )
		, m_particles( true )
		, m_sound( true )
		, m_debugMask( 0 )
		, m_phycisTiming(0.f)
		, m_resetTiming( false )
		, m_maxTiming( 100.0f )
		, m_debugInfo( false )
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
		, m_userLocal( _userLocal )
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
	{
		Holder<Application>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		finalize();
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
		Holder<Game>::keep( m_game );

		MENGE_LOG( "Create game file \"%s\""
			, m_gameInfo.c_str() );

		m_fileEngine->loadPak( m_gamePack );

		if( m_xmlEngine->parseXmlFileM( m_gamePack + "/" + m_gameInfo, m_game, &Game::loader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid game file \"%s\""
				, m_gameInfo.c_str() );
			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		/*if( m_game->loader( m_gamePack + "/" + m_gameInfo ) == false )
		{
			MENGE_LOG_ERROR( "Invalid game file \"%s\""
				, m_gameInfo.c_str() );

			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );

			return false;
		}*/

		m_game->setBaseDir( m_baseDir );
		m_game->loadConfigPaks();
		//m_game->registerResources( m_baseDir );
		
		String title = m_game->getTitle();
		bool fullscreen = m_game->getFullscreen();
		m_renderEngine->setFullscreenMode( fullscreen );

		if( m_userPath.empty() == true )
		{
			m_userPath = "Menge/" + title;
		}

		m_fileEngine->initAppDataPath( m_userPath, m_userLocal );

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

		//WindowHandle winHandle = _handle;

		/*if( _handle == NULL )
		{
			winHandle = m_interface->createWindow( 
				title, m_currentResolution.getWidth(), m_currentResolution.getHeight(), isFullscreen );
		}*/

		int bits = m_game->getBits();
		int FSAAType = m_game->getFSAAType();
		int FSAAQuality = m_game->getFSAAQuality();

		bool res = m_renderEngine->createRenderWindow( m_currentResolution, bits, isFullscreen, _renderWindowHandle,
											FSAAType, FSAAQuality );
		if( res == false )
		{
			showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}

		//bool isTextureFiltering = m_game->getTextureFiltering();

		//m_renderEngine->setTextureFiltering( isTextureFiltering );

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
		bool result = m_game->init();
		if( result )
		{
			m_game->update( 0.0f );
		}
		return result;
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
			XML_CASE_ATTRIBUTE_NODE( "GamePack", "Path", m_gamePack );
			XML_CASE_ATTRIBUTE_NODE( "Game", "File", m_gameInfo );
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

		idx = _arguments.find( "-debuginfo" );
		if( idx != String::npos )
		{
			m_debugInfo = true;
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
	LogSystemInterface * Application::getLogSystem() const
	{
		return m_logSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderSystemInterface * Application::getRenderSystem() const
	{
		return m_renderSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	LogSystemInterface* Application::initializeLogSystem()
	{
		initInterfaceSystem( &m_logSystem );
		this->setLogSystem( m_logSystem );
		return m_logSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const String& _applicationFile, const String & _args, bool _loadPersonality )
	{
		parseArguments_( _args );

		//String loc = setlocale( LC_CTYPE, NULL ); // default (OS) locale

		Holder<SceneManager>::keep( new SceneManager() );
		Holder<ResourceManager>::keep( new ResourceManager() );
		Holder<TextManager>::keep( new TextManager() );
		//Holder<Console>::keep( new Console() );
		

		parseArguments_( _args );

		MENGE_LOG( "Inititalizing File System..." );
		initInterfaceSystem( &m_fileSystem );
		m_fileSystem->inititalize( m_logSystem );
		m_fileLog = m_fileSystem->openOutStream( "Menge.log", false );
		if( m_fileLog != NULL )
		{
			m_logSystem->registerLogger( m_fileLog );
			m_logSystem->logMessage( "Starting log to Menge.log\n" );
		}
		this->setFileSystem( m_fileSystem );

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

		res = m_renderEngine->initialize();

		if( res == false )
		{
			showMessageBox( "Failed to initialize Render System", "Crititcal Error", 0 );
			return false;
		}

		MENGE_LOG( "Initializing Thread System..." );
		initInterfaceSystem( &m_threadSystem );
		m_threadManager = new ThreadManager( m_threadSystem );
		Holder<ThreadManager>::keep( m_threadManager );
		m_taskManager = new TaskManager();
		Holder<TaskManager>::keep( m_taskManager );

		MENGE_LOG( "Creating Object Factory..." );
		OBJECT_FACTORY( Camera2D );
		OBJECT_FACTORY( Entity );
		OBJECT_FACTORY( Animation );
		OBJECT_FACTORY( Arrow );
		OBJECT_FACTORY( Emitter );
		OBJECT_FACTORY( HotSpot );
		OBJECT_FACTORY( Light2D );
		OBJECT_FACTORY( ShadowCaster2D );
		OBJECT_FACTORY( TilePolygon );
		OBJECT_FACTORY( Point );
		OBJECT_FACTORY( RigidBody2D );
		OBJECT_FACTORY( SoundEmitter );
		OBJECT_FACTORY( Sprite );
		OBJECT_FACTORY( TextField );
		OBJECT_FACTORY( TileMap );
		OBJECT_FACTORY( Track );
		OBJECT_FACTORY( Video );
		OBJECT_FACTORY( Layer2D );
		OBJECT_FACTORY( Layer2DLoop );
		OBJECT_FACTORY( Layer2DAccumulator );
		OBJECT_FACTORY( Layer3D );
		OBJECT_FACTORY( LayerScene );
		OBJECT_FACTORY( RenderMesh );
		OBJECT_FACTORY( Camera3D );
		OBJECT_FACTORY( SceneNode3D );
		OBJECT_FACTORY( Window );
		OBJECT_FACTORY( HotSpotImage );

		MENGE_LOG( "Creating Resource Factory..." );
		RESOURCE_FACTORY( ResourceAnimation );
		//RESOURCE_FACTORY( ResourceCapsuleController );
		RESOURCE_FACTORY( ResourceEmitterContainer );
		RESOURCE_FACTORY( ResourceFont );
		RESOURCE_FACTORY( ResourceImageAtlas );
		RESOURCE_FACTORY( ResourceTilePolygon );
		RESOURCE_FACTORY( ResourceImageCell );
		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageDynamic );
		RESOURCE_FACTORY( ResourceImageSet );
		RESOURCE_FACTORY( ResourceVideo );
		RESOURCE_FACTORY( ResourceMesh );
		//RESOURCE_FACTORY( ResourceSkeleton );
		//RESOURCE_FACTORY( ResourcePhysicBoxGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConcaveGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConvexGeometry );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceTileMap );
		RESOURCE_FACTORY( ResourceTileSet );
		RESOURCE_FACTORY( ResourceMeshMS3D );
		RESOURCE_FACTORY( ResourceMeshNoise );
		RESOURCE_FACTORY( ResourceMaterial );
		RESOURCE_FACTORY( ResourceWindow );
		RESOURCE_FACTORY( ResourceHotspotImage );

		if( m_interface != NULL )
		{
			m_desktopResolution[0] = m_interface->getDesktopWidth();
			m_desktopResolution[1] = m_interface->getDesktopHeight();
		}

		//MENGE_LOG_CRITICAL( MENGE_TEXT("BEGIN") );

		MENGE_LOG( "Initializing Xml Engine..." );
		m_xmlEngine = new XmlEngine();
		Holder<XmlEngine>::keep( m_xmlEngine );

		if( m_xmlEngine->parseXmlFileM( _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG_ERROR( "parse application xml failed \"%s\""
				, _applicationFile.c_str() );
			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		if( m_baseDir.empty() == false )
		{
			m_fileEngine->loadPath( m_baseDir );
		}

		MENGE_LOG( "Initializing Script Engine..." );

		m_scriptEngine = new ScriptEngine();
		Holder<ScriptEngine>::keep( m_scriptEngine );
		m_scriptEngine->init();

		MENGE_LOG( "Inititalizing Codecs..." );

		Holder<DecoderManager>::keep( new DecoderManager() );
		Holder<EncoderManager>::keep( new EncoderManager() );

		//Holder<Console>::hostage()->inititalize( m_logSystem );
		// Decoders
		//MENGE_REGISTER_DECODER( "Image", ImageDecoderMNE, "mne" );

		MENGE_REGISTER_DECODER( "Image", ImageDecoderPNG, "png" );
		MENGE_REGISTER_DECODER( "Image", ImageDecoderJPEG, "jpeg" );
		MENGE_REGISTER_DECODER( "Image", ImageDecoderJPEG, "jpg" );
		MENGE_REGISTER_DECODER( "Image", ImageDecoderMNE, "mne" );
		VideoDecoderOGGTheora::createCoefTables_();
		MENGE_REGISTER_DECODER( "Video", VideoDecoderOGGTheora, "ogg" );
		MENGE_REGISTER_DECODER( "Video", VideoDecoderOGGTheora, "ogv" );
		MENGE_REGISTER_DECODER( "Sound", SoundDecoderOGGVorbis, "ogg" );
		MENGE_REGISTER_DECODER( "Sound", SoundDecoderOGGVorbis, "ogv" );
		// Encoders
		MENGE_REGISTER_ENCODER( "Image", ImageEncoderPNG, "png" );
		
		loadPlugins_("");

		loadGame( _loadPersonality );

		if( m_console != NULL )
		{
			m_console->inititalize( m_logSystem );
		}

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

		if( ( _key == 0x1C || _key == 0x9C ) && _isDown && ( m_altDown > 0 ) ) // Enter
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
		if( _key == 87 && _isDown && m_enableDebug ) // F11
		{
			Holder<Player>::hostage()
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
		if( m_focus == _focus ) return;

		//static float volume = 1.0f;
		//static float avolume = 1.0f;

		//if( !_focus )
		//{
		//	volume = Holder<SoundEngine>::hostage()->getCommonVolume();
		//	avolume = Holder<Amplifier>::hostage()->getVolume();
		//	m_soundEngine->setCommonVolume( 0.0f );
		//	Holder<Amplifier>::hostage()->setVolume( 0.0f );
		//}
		//else
		//{
		//	m_resetTiming = true;
		//	m_soundEngine->setCommonVolume( volume );
		//	Holder<Amplifier>::hostage()->setVolume( avolume );
		//}
		Holder<SoundEngine>::hostage()
			->onFocus( _focus );

		m_focus = _focus;
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
			return;
		}

		m_renderEngine->beginScene();

		m_game->render( m_debugMask );

		if( m_console != NULL )
		{
			m_console->render();
		}
		//Holder<Console>::hostage()->render();

		m_renderEngine->endScene();
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

		Holder<Game>::destroy();

		Holder<DecoderManager>::destroy();
		Holder<EncoderManager>::destroy();

		//Holder<Console>::destroy();
		Holder<TextManager>::destroy();
		Holder<SceneManager>::destroy();

		Holder<ResourceManager>::destroy();
		Holder<ScriptEngine>::destroy();

		Holder<PhysicEngine>::destroy();
		Holder<PhysicEngine2D>::destroy();
		Holder<ParticleEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<XmlEngine>::destroy();
		Holder<LogEngine>::destroy();

 		if( m_taskManager != NULL )
 		{
 			delete m_taskManager;
 			m_taskManager = NULL;
 		}
 		Holder<TaskManager>::empty();
		if( m_threadManager != NULL )
		{
			delete m_threadManager;
			m_threadManager = NULL;
		}
		Holder<ThreadManager>::empty();
		releaseInterfaceSystem( m_threadSystem );

		releaseInterfaceSystem( m_soundSystem );
		releaseInterfaceSystem( m_renderSystem );
		releaseInterfaceSystem( m_physicSystem2D );
//#	if	MENGE_PARTICLES	== (1)
		releaseInterfaceSystem( m_particleSystem );
//#	endif
		releaseInterfaceSystem( m_inputSystem );

		if( m_fileLog != NULL && m_logSystem != NULL )
		{
			m_logSystem->unregisterLogger( m_fileLog );
			m_fileSystem->closeOutStream( m_fileLog );
			m_fileLog = NULL;
		}


		releaseInterfaceSystem( m_fileSystem );
		releaseInterfaceSystem( m_logSystem );
		//		releaseInterfaceSystem( m_profilerSystem );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onWindowMovedOrResized()
	{
		m_renderEngine->onWindowMovedOrResized();
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
			}
			else
			{
				m_interface->setHandleMouse( false );
				m_inputEngine->setMouseBounded( true );
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
			m_inputEngine->setMousePos( _x, _y );
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
}

