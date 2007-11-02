#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ScriptEngine.h"

#	include "MousePickerSystem.h"
#	include "ResourceManager.h"

#	include "Game.h"

#	include "Player.h"

#	include "ErrorMessage.h"

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
		{
		}

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
	{
		m_handler = new ApplicationInputHandlerProxy( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		delete m_handler;

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<ScriptEngine>::destroy();
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
	void Application::loadPak( const std::string & _pak )
	{
		printf("load pack [%s]...\n", _pak.c_str() );
		Holder<FileEngine>::hostage()->loadPak( _pak );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame( const std::string & _game )
	{
		printf("create game file [%s] ...\n", _game.c_str() );

		Holder<Game>::keep( new Game );

		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( _game );

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
			printf("Invalid game file [%s] ...\n", _game.c_str() );
			return false;
		}

		printf("init game ...\n");

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

		printf("init scriptEngine ...\n");
		scriptEngine->init();

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		inputEng->regHandle( m_handler );

		Holder<ResourceManager>::keep( new ResourceManager );

		std::string gameFile;

		TiXmlDocument * xmlDocument = 
			Holder<FileEngine>::hostage()
			->loadXml( _applicationFile );
		
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

				XML_CHECK_VALUE_NODE("Game", "File", m_gameInfo );
			}
		}
		XML_INVALID_PARSE()
		{
			//TODO: ERROR
			printf("parse application xml failed\n");
			return false;
		}

		bool creating = createGame( m_gameInfo );

		return creating;
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
	bool Application::update( size_t _timing )
	{	
		Holder<MousePickerSystem>::hostage()->clear();
		Holder<Game>::hostage()->update( _timing );
		Holder<InputEngine>::hostage()->update();
		Holder<MousePickerSystem>::hostage()->update();

		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		Holder<Game>::hostage()->render();
		Holder<Game>::hostage()->debugRender();

		renderEng->render();

		return Holder<InputEngine>::hostage()->isKeyDown( 0x01 ) == false;
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
		return Holder<Game>::hostage()->handleMouseMove( _x, _y, _whell );
	}
}
