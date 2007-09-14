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

//#	include "E:\\Menge\\dependencies\\vld\\vld.h"

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

		bool handleMouseMove( float _x, float _y, float _whell ) override
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
	void Application::loadPak( const std::string & _pak, int _priority )
	{
		printf("load pack [%s] priority - %d ...\n", _pak.c_str(), _priority );
		Holder<FileEngine>::hostage()->loadPak( _pak, _priority );
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
	bool Application::init()
	{
		Holder<ScriptEngine>::keep( new ScriptEngine );

		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();

		printf("init scriptEngine ...\n");
		scriptEngine->init();

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		inputEng->regHandle( m_handler );

		inputEng->setPosition(0.f, 0.f, 0.f);
		inputEng->setSensitivity( 1.f );
		mt::vec3f minRange(0,0,-1000);
		mt::vec3f maxRange(1024,768,1000);
		inputEng->setRange( minRange, maxRange );

		Holder<ResourceManager>::keep( new ResourceManager );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::fini()
	{
		Holder<Game>::hostage()->release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loop()
	{	
		update();
		render();
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
	bool Application::handleMouseMove( float _x, float _y, float _whell )
	{
		return Holder<Game>::hostage()->handleMouseMove( _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::update()
	{
		Holder<MousePickerSystem>::hostage()->clear();
		Holder<Game>::hostage()->update(0.01f);
		Holder<InputEngine>::hostage()->update();
		Holder<MousePickerSystem>::hostage()->update();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::render()
	{
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();
	
		renderEng->beginScene(2756);

		Holder<Game>::hostage()->render();
		Holder<Game>::hostage()->debugRender();

		renderEng->endScene();
	}
}
