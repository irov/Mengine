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

#	include "XmlParser.h"

#	include "Decoder.h"

#	include "osdefs.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_game(0)
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
	//////////////////////////////////////////////////////////////////////////
	bool Application::init(const std::string &_xmlFile)
	{
		Holder<ScriptEngine>::keep( new ScriptEngine );

		ScriptEngine * scriptEngine = Holder<ScriptEngine>::hostage();
		scriptEngine->init();

		XML_PARSE_FILE("Application.xml")
		{
			XML_CHECK_NODE_FOR_EACH("Application")
			{
				XML_CHECK_NODE_FOR_EACH("DllModule")
				{
					XML_DEF_ATTRIBUTES_NODE(DllFile);
 
					if( DllFile.empty() )
					{
						XML_CONTINUE();
					}

					XML_CHECK_NODE("FileSystem")
					{
						Holder<FileEngine>::keep( new FileEngine(DllFile) );			
					}

					XML_CHECK_NODE("InputSystem")
					{
						Holder<InputEngine>::keep( new InputEngine(DllFile) );
					}

					XML_CHECK_NODE("RenderSystem")
					{
						Holder<RenderEngine>::keep( new RenderEngine(DllFile) );
					}

					XML_CHECK_NODE("SoundSystem")
					{
						//Holder<SoundEngine>::keep( new SoundEngine(DllFile) ); 
					}

					XML_CHECK_NODE("Codec")
					{
					//	Decoder::DecoderComponent * dc = new Decoder::DecoderComponent(DllFile);
						//dc->
					//	Decoder::regDecoder("mng",dc);
						//m_soundEngine = new SoundEngine(DllFile); 
					}
				}

				XML_CHECK_NODE_FOR_EACH("Paks")
				{
					XML_CHECK_NODE("Pak")
					{
						std::string file;
						int priority;

						XML_VALUE_ATTRIBUTE("File",file);
						XML_VALUE_ATTRIBUTE("Priority",priority);

						m_packHierarchical[ priority ].push_back( file );
					}
				}

				XML_CHECK_NODE_FOR_EACH("Display")
				{
					XML_CHECK_VALUE_NODE("Width","Value",m_width);
					XML_CHECK_VALUE_NODE("Height","Value",m_height);
					XML_CHECK_VALUE_NODE("Bits","Value",m_bits);
					XML_CHECK_VALUE_NODE("FullScreen","Value",m_fullScreen);
				}

				XML_CHECK_NODE("Game")
				{
					XML_DEF_ATTRIBUTES_NODE(File);

					if( File.empty() )
					{
						continue;
					}

					m_game = new Game();

					XML_PARSE_FILE_EX( File )
					{
						XML_CHECK_NODE("Game")
						{
							m_game->loader(XML_CURRENT_NODE);
						}
					}
					
				}
			}
		}
		XML_INVALID_PARSE()
		{
			//TODO: ERROR
			return false;
		}

		const std::string & pathEntities = m_game->getPathEntities();
		std::string scriptPath = pathEntities;

		for each( const TPackHierarchical::value_type & it in m_packHierarchical )
		{
			for each( const std::string & pak in it.second )
			{
				Holder<FileEngine>::hostage()->loadPak( pak, it.first );
				//scriptPath += DELIM;
				//scriptPath += pak;
				//scriptPath += pathEntities;
			}
		}

		Holder<ScriptEngine>::hostage()
			->setEntitiesPath( scriptPath );

		//»«-«¿ À»Õ»… Õ≈“” ¬Œ——“¿ÕŒ¬À≈Õ»ﬂ –≈—”–—Œ¬.
		bool display = createDisplay(m_width,m_height,m_bits,m_fullScreen);

		if( display == false )
		{
			//TODO: ERROR
		}

		InputEngine * inputEng = Holder<InputEngine>::hostage();

		bool inputInit = false;

		if (inputEng)
		{
			inputEng->setPosition(0.f, 0.f, 0.f);
			inputEng->setSensitivity( 1.f );
			mt::vec3f minRange(0,0,-1000);
			mt::vec3f maxRange(1024,768,1000);
			inputEng->setRange( minRange, maxRange );
			
			inputInit = inputEng->init();
		}

		if(inputInit == false)
		{
			ErrorMessage("Input Manager invalid initialization");
		}

		Holder<ResourceManager>::keep( new ResourceManager );

		m_game->init();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::run()
	{	
		MSG msg = MSG();
		while(msg.message != WM_QUIT)
		{
			if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{			
				update();
				render();
			}
		}
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

		renderEng->beginSceneDrawing(0xFF0FF0FF);

		//if( m_functionRender )
		//{
		//	m_functionRender->callFunctionVoid();
		//}
		m_game->render();
		m_game->debugRender();

		renderEng->endSceneDrawing();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createDisplay(int _width, int _height, int _bits, bool _fullScreen)
	{
		return Holder<RenderEngine>::hostage()->createDisplay(_width,_height,_bits,_fullScreen);
	}
}