#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ParticleEngine.h"
#	include "ScriptEngine.h"

#	include "MousePickerSystem.h"
#	include "ResourceManager.h"

#	include "Game.h"

#	include "Player.h"

#	include "LogEngine.h"

#	include "XmlParser/XmlParser.h"

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
		bool handleKeyEvent( size_t _key, bool _isDown ) override
		{
			return m_app->handleKeyEvent( _key, _isDown );
		}

		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override
		{
			return m_app->handleMouseButtonEvent( _button, _isDown );
		}

		bool handleMouseMove( int _x, int _y, int _whell ) override
		{
			return m_app->handleMouseMove( _x, _y, _whell );
		}

	private:
		Application * m_app;
	};

	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_quit( false )
	{
		Holder<Application>::keep( this );
		m_handler = new ApplicationInputHandlerProxy( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		delete m_handler;

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();
		Holder<ParticleEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<ScriptEngine>::destroy();
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

		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( m_gameInfo );

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE("Game")
			{
				Holder<Game>::hostage()
					->loader(XML_CURRENT_NODE);
			}
		}
		XML_INVALID_PARSE()
		{
			MENGE_LOG("Invalid game file [%s] ...\n", m_gameInfo.c_str() );
			return false;
		}

		MENGE_LOG("init game ...\n");

		if( Holder<Game>::hostage()
			->init() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const std::string & _applicationFile )
	{
		Holder<ScriptEngine>::keep( new ScriptEngine );

		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();

		MENGE_LOG("init scriptEngine ...\n");
		scriptEngine->init();

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		inputEng->regHandle( m_handler );

		Holder<ResourceManager>::keep( new ResourceManager );

		TiXmlDocument * xmlDocument = 
			Holder<FileEngine>::hostage()
			->loadXml( _applicationFile );

		if( xmlDocument == NULL )
		{
			return false;
		}
		
		XML_FOR_EACH_DOCUMENT( xmlDocument )
		{
			XML_CHECK_NODE_FOR_EACH("Application")
			{
				XML_CHECK_NODE_FOR_EACH("Paks")
				{
					XML_CHECK_NODE("Pak")
					{
						XML_DEF_ATTRIBUTES_NODE( File );

						loadPak( File );
					}
				}

				XML_CHECK_NODE_FOR_EACH("Config")
				{
					XML_CHECK_VALUE_NODE("Width", "Value", m_width );					
					XML_CHECK_VALUE_NODE("Height", "Value", m_height );
					XML_CHECK_VALUE_NODE("Bits", "Value", m_bits );
					XML_CHECK_VALUE_NODE("Fullscreen", "Value", m_fullScreen );
					XML_CHECK_VALUE_NODE("RenderDriver", "Name", m_renderDriver );
				}

				XML_CHECK_VALUE_NODE("Game", "File", m_gameInfo );
			}
		}
		XML_INVALID_PARSE()
		{
			MENGE_LOG("parse application xml failed\n");
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
	bool Application::update( float _timing )
	{	
		Holder<MousePickerSystem>::hostage()->clear();
		Holder<Game>::hostage()->update( _timing );
		Holder<InputEngine>::hostage()->update();
		Holder<MousePickerSystem>::hostage()->update();

		Holder<Game>::hostage()->render();
		Holder<Game>::hostage()->debugRender();

		return !m_quit;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleKeyEvent( size_t _key, bool _isDown )
	{
		return Holder<Game>::hostage()->handleKeyEvent( _key, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return Holder<Game>::hostage()->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleMouseMove( int _x, int _y, int _whell )
	{
		Holder<InputEngine>::hostage()->setMousePos( _x, _y );
		return Holder<Game>::hostage()->handleMouseMove( _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::handleMouseLeave()
	{
		Holder<Game>::hostage()->handleMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::handleMouseEnter()
	{
		Holder<Game>::hostage()->handleMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		m_quit = true; 
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::frameEnded()
	{}
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
}
