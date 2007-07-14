#	include "Application.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ScriptEngine.h"

#	include "ResourceManager.h"

#	include "Game.h"

#	include "Player.h"

#	include "ErrorMessage.h"

#	include "Decoder.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_game(0)
		, m_fullScreen(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		if (m_game)
		{
			m_game->release();
		}

		delete m_game;

		Holder<FileEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<ScriptEngine>::destroy();
		Holder<ResourceManager>::destroy();
	}

	/*void	Application::loadPlugin(const std::string& _name)
	{
		void *m_hInst;
		typedef void (*DLL_START_PLUGIN)(void);

		m_hInst = LoadLibrary(_name.c_str());
		DLL_START_PLUGIN initialize = (DLL_START_PLUGIN)GetProcAddress((HMODULE)m_hInst, "dllStartPlugin" );
		initialize();
	}
	*/
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
		//TODO
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
	void Application::update()
	{
		//if( m_functionUpdate )
		//{
		//	m_functionUpdate->callFunctionVoid();
		//}

		Holder<InputEngine>::hostage()->update();
		m_game->update(0.001f);
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::render()
	{
		RenderEngine *renderEng = Holder<RenderEngine>::hostage();

	
		renderEng->beginScene(true,true,2756);
		
		//if( m_functionRender )
		//{
		//	m_functionRender->callFunctionVoid();
		//}
		m_game->render();

		m_game->debugRender();
		

		renderEng->endScene();
	}
}
