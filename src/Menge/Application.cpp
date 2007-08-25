#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ScriptEngine.h"

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
		: m_game(0)
	{
		m_handler = new ApplicationInputHandlerProxy( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		if (m_game)
		{
			m_game->release();
		}

		delete m_handler;
		delete m_game;

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
		m_game = new Game();

		TiXmlDocument * document = Holder<FileEngine>::hostage()
			->loadXml( _game );

		XML_FOR_EACH_DOCUMENT( document )
		{
			XML_CHECK_NODE("Game")
			{
				m_game->loader(XML_CURRENT_NODE);
			}
		}
		XML_INVALID_PARSE()
		{
			printf("Invalid game file [%s] ...\n", _game.c_str() );
			return false;
		}

		const std::string & pathEntities = m_game->getPathEntities();
		std::string scriptPath = pathEntities;

		printf("set script Entity path [%s] ...\n", scriptPath.c_str() );

		Holder<ScriptEngine>::hostage()
			->setEntitiesPath( scriptPath );

		printf("init game ...\n");

		if( m_game->init() == false )
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
	void Application::loop()
	{	
		update();
		render();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleKeyEvent( size_t _key, bool _isDown )
	{
		if( m_game )
		{
			return m_game->handleKeyEvent( _key, _isDown );
		}		
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		if( m_game )
		{
			return m_game->handleMouseButtonEvent( _button, _isDown );
		}		

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::handleMouseMove( float _x, float _y, float _whell )
	{
		if( m_game )
		{
			return m_game->handleMouseMove( _x, _y, _whell );
		}	

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::update()
	{
		//if( m_functionUpdate )
		//{
		//	m_functionUpdate->callFunctionVoid();
		//}

		Holder<InputEngine>::hostage()->update();
		m_game->update(0.01f);
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::render()
	{
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

	
		renderEng->beginScene(2756);
		
		//if( m_functionRender )
		//{
		//	m_functionRender->callFunctionVoid();
		//}
		m_game->render();

		m_game->debugRender();

		renderEng->endScene();
	}
}
