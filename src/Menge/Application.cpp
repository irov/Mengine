#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ParticleEngine.h"
#	include "ScriptEngine.h"
#	include "PhysicEngine.h"

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
		, m_quit( false )
		, m_commandLine("")
		, m_fixedContentResolution( false )
		, m_title("Menge-engine")
		, m_resourcePath("")
		, m_vsync( true )
		, m_particles( true )
		, m_sound( true )
		, m_usePhysic( false )
		, m_logSystemName( "OgreLogSystem" )
		, m_renderSystemName( "OgreRenderSystem" )
		, m_inputSystemName( "OgreInputSystem" )
		, m_soundSystemName( "ALSoundSystem" )
		, m_particleSystemName( "None" )
		, m_physicSystemName( "None" )
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
	void Application::setPhysicSystem( PhysicSystemInterface * _interface )
	{
		new PhysicEngine( _interface );
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

		Holder<Game>::keep( new Game );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( m_gameInfo, Holder<Game>::hostage(), &Game::loader ) == false )
		{
			MENGE_LOG("Invalid game file [%s] ...\n", m_gameInfo.c_str() );
			return false;
		}

		MENGE_LOG("init game ...\n");

		if( m_usePhysic == true )
		{
			Holder<PhysicEngine>::hostage()->init(mt::vec3f(0,-1.0,0));
		}

		if( Holder<Game>::hostage()
			->init() == false )
		{
			return false;
		}

		if( m_fixedContentResolution )
		{
			mt::vec2f res = Holder<Game>::hostage()->getResourceResolution();
			Holder<RenderEngine>::hostage()->setViewportDimensions( res.x, res.y );
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

			XML_CASE_ATTRIBUTE_NODE( "ResourcePath", "Path", m_resourcePath );

			XML_CASE_ATTRIBUTE_NODE( "Game", "File", m_gameInfo );

			XML_CASE_NODE("Config")
			{
				XML_PARSE_ELEMENT( this, &Application::loaderConfig );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loaderConfig( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			/*XML_CASE_ATTRIBUTE_NODE("Title", "Name", m_title );					
			XML_CASE_ATTRIBUTE_NODE("RenderDriver", "Name", m_renderDriver );
			XML_CASE_ATTRIBUTE_NODE("FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE("VSync", "Value", m_vsync );
			XML_CASE_ATTRIBUTE_NODE("UsePhysic", "Value", m_usePhysic );*/
			XML_CASE_ATTRIBUTE_NODE("LogSystem", "Name", m_logSystemName );
			XML_CASE_ATTRIBUTE_NODE("InputSystem", "Name", m_inputSystemName );
			XML_CASE_ATTRIBUTE_NODE("RenderSystem", "Name", m_renderSystemName );
			XML_CASE_ATTRIBUTE_NODE("SoundSystem", "Name", m_soundSystemName );
			XML_CASE_ATTRIBUTE_NODE("ParticleSystem", "Name", m_particleSystemName );
			XML_CASE_ATTRIBUTE_NODE("PhysicSystem", "Name", m_physicSystemName );
			XML_CASE_ATTRIBUTE_NODE("RenderDriver", "Name", m_renderDriver );
			XML_CASE_ATTRIBUTE_NODE("Width", "Value", m_width );					
			XML_CASE_ATTRIBUTE_NODE("Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE("Bits", "Value", m_bits );
			XML_CASE_ATTRIBUTE_NODE("Fullscreen", "Value", m_fullScreen );
			XML_CASE_ATTRIBUTE_NODE("VSync", "Value", m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const std::string & _applicationFile, const std::string& _args )
	{

		if( !m_interface->init( _applicationFile.c_str(), _args.c_str(), this ) )
		{
			return false;
		}

		int idx = _args.find( "-sound" );
		if( idx >= 0 )
		{
			m_sound = false;
		}
		idx = _args.find( "-particles" );
		if( idx >= 0 )
		{
			m_particles = false;
		}
		
		setFileSystem( m_interface->getFileSystemInterface() );

		Holder<XmlEngine>::keep( new XmlEngine );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG("parse application xml failed '%s'\n"
				, _applicationFile.c_str()
				);
		}

		if( !_initSystems() )
		{
			return false;
		}

		WINDOW_HANDLE winHandle = m_interface->createWindow( "window name", m_width, m_height, m_fullScreen );
		Holder<RenderEngine>::hostage()->initialize( m_renderDriver, m_width, m_height, m_bits, m_fullScreen, winHandle );
		Holder<InputEngine>::hostage()->initialize( winHandle );

		Holder<ScriptEngine>::keep( new ScriptEngine );

		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();

		MENGE_LOG("init scriptEngine ...\n");
		scriptEngine->init();

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		inputEng->regHandle( m_handler );

		Holder<ResourceManager>::keep( new ResourceManager );

		Holder<FileEngine>::hostage()->changeDir( m_resourcePath );
		createGame();

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
	bool Application::update( float _timing )
	{	
		Holder<MousePickerSystem>::hostage()->clear();

		if( m_usePhysic == true )
		{
			Holder<PhysicEngine>::hostage()->update( 1.0f/30.0f );// for test physic!
		}

		Holder<Game>::hostage()->update( _timing );
		Holder<InputEngine>::hostage()->update();
		Holder<MousePickerSystem>::hostage()->update();
		Holder<SoundEngine>::hostage()->update();

		Holder<Game>::hostage()->render();
		Holder<Game>::hostage()->debugRender();
		Holder<RenderEngine>::hostage()->render();

		return !m_quit;
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
		int oldx = inpEng->getMouseX();
		int oldy = inpEng->getMouseY();
		inpEng->setMousePos( _x, _y );
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
	int Application::getScreenWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	int Application::getScreenHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	int Application::getScreenBits() const
	{
		return m_bits;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::isFullScreen() const
	{
		return m_fullScreen;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& Application::getRenderDriver() const
	{
		return m_renderDriver;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& Application::getTitle() const
	{
		return m_title;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& Application::getResourcePath() const
	{
		return m_resourcePath;
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
	bool Application::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::usePhysic() const
	{
		return m_usePhysic;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onFocus( bool _focus )
	{
		static float volume = 1.0f;
		if( !_focus )
		{
			volume = Holder<SoundEngine>::hostage()->getCommonVolume();
			Holder<SoundEngine>::hostage()->setCommonVolume( 0.0f );
		}
		else
		{
			Holder<SoundEngine>::hostage()->setCommonVolume( volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::minimizeWindow()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::_initSystems()
	{
		SystemDLLInterface* dll = NULL;

		// LogSystem
		dll = m_interface->loadSystemDLL( m_logSystemName.c_str() );
		setLogSystem( dll->getInterface<LogSystemInterface>() );
		m_systemDLLs.push_back( dll );

		// RenderSystem
		dll = m_interface->loadSystemDLL( m_renderSystemName.c_str() );
		setRenderSystem( dll->getInterface<RenderSystemInterface>() );
		m_systemDLLs.push_back( dll );

		// InputSystem
		dll = m_interface->loadSystemDLL( m_inputSystemName.c_str() );
		setInputSystem( dll->getInterface<InputSystemInterface>() );
		m_systemDLLs.push_back( dll );

		// SoundSystem
		dll = m_interface->loadSystemDLL( m_soundSystemName.c_str() );
		setSoundSystem( dll->getInterface<SoundSystemInterface>() );
		m_systemDLLs.push_back( dll );

		// ParticleSystem
		if( m_particleSystemName != "None" )
		{
			dll = m_interface->loadSystemDLL( m_particleSystemName.c_str() );
			setParticleSystem( dll->getInterface<ParticleSystemInterface>() );
			m_systemDLLs.push_back( dll );
		}
		
		// PhysicSystem
		if( m_physicSystemName != "None" )
		{
			dll = m_interface->loadSystemDLL( m_physicSystemName.c_str() );
			setPhysicSystem( dll->getInterface<PhysicSystemInterface>() );
			m_systemDLLs.push_back( dll );
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::run()
	{
		m_interface->run();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onUpdate( float _timing )
	{
		//printf( "%.4f\n", _timing );
		update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onClose()
	{
		m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onDestroy()
	{
		delete m_handler;

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();

		if ( m_usePhysic == true )
		{
			Holder<PhysicEngine>::destroy();
		}

		Holder<ParticleEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<ScriptEngine>::destroy();

		for( TSystemDLLVector::iterator it = m_systemDLLs.begin()
			,it_end = m_systemDLLs.end();
			it != it_end;
		it++ )
		{
			m_interface->unloadSystemDLL( (*it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onWindowMovedOrResized()
	{

	}
}