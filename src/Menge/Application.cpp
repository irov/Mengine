#	include "Application.h"

#	include <list>

#	include "Keeper.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "ScriptEngine.h"

#	include "Game.h"
#	include "Player.h"

#	include "ErrorMessage.h"
#	include "XmlParser.h"

#	define WIN32_LEAN_AND_MEAN
#	include "windows.h"

namespace Menge
{
	Application::Application()
		: m_game(0)
	{

	}
	Application::~Application()
	{
		m_game->release();
		delete m_game;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::init(const std::string &_xmlFile)
	{
		ScriptEngine *scriptEngine = new ScriptEngine;
		
		scriptEngine->init();

		typedef std::list< std::pair<std::string,int> > TListLoadPaks;
		TListLoadPaks listLoadPaks;

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
						new FileEngine(DllFile);			
					}

					XML_CHECK_NODE("InputSystem")
					{
						new InputEngine(DllFile);
					}

					XML_CHECK_NODE("RenderSystem")
					{
						new RenderEngine(DllFile);
					}

					XML_CHECK_NODE("SoundSystem")
					{
						new SoundEngine(DllFile);
					}
				
					XML_CHECK_NODE("PhysSystem")
					{
//						new PhysicEngine(DllFile);					
					}
				}

				XML_CHECK_NODE_FOR_EACH("Paks")
				{
					//<Pak File = "Data.pak" Priority = "1" />
					XML_CHECK_NODE("Pak")
					{
						std::string file;
						int priority;

						XML_VALUE_ATTRIBUTE("File",file);
						XML_VALUE_ATTRIBUTE("Priority",priority);

						listLoadPaks
							.push_back(std::make_pair(file,priority));
					}
				}

				//<Display>
				//	<Width Value = "1024"/>
				//	<Height Value = "768"/>
				//	<Bits Value = "32"/>
				//	<FullScreen Value = "0"/>
				//	</Display>
				XML_CHECK_NODE_FOR_EACH("Display")
				{
					XML_CHECK_VALUE_NODE("Width","Value",m_width);
					XML_CHECK_VALUE_NODE("Height","Value",m_height);
					XML_CHECK_VALUE_NODE("Bits","Value",m_bits);
					XML_CHECK_VALUE_NODE("FullScreen","Value",m_fullScreen);
				}

				//<Game File = "Game/Game.xml" />
				XML_CHECK_NODE("Game")
				{
					XML_DEF_ATTRIBUTES_NODE(File);

					m_game = new Game();

					m_game->setResource(File);
				}
			}
		}
		XML_INVALID_PARSE()
		{
			//TODO: ERROR
			return false;
		}

		for( TListLoadPaks::iterator
			it = listLoadPaks.begin(),
			it_end = listLoadPaks.end();
		it != it_end;
		++it)
		{
			Keeper<FileEngine>::hostage()->loadPak(it->first,it->second);
		}

		//��-�� ����� ���� �������������� ��������.
		bool display = createDisplay(m_width,m_height,m_bits,m_fullScreen);

		if( display == false )
		{
			//TODO: ERROR
		}

		InputEngine *inputEng = Keeper<InputEngine>::hostage();

		inputEng->SetPositionAndSpeed (0, 0, 0, 1);
		inputEng->SetRange (0, 0, -1000, 1024, 768, 1000);

		bool inputInit = inputEng->Init();

		if( inputInit == false )
		{
			ErrorMessage("Input Manager invalid initialization");
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::run()
	{	
		m_game->activate();

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

		Keeper<InputEngine>::hostage()->Update();

		m_game->update(0.07f);
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::render()
	{
		RenderEngine *renderEng = Keeper<RenderEngine>::hostage();

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
	bool Application::createDisplay(
		unsigned int _width, 
		unsigned int _height, 
		unsigned int _bits, 
		bool _fullScreen)
	{
		return Keeper<RenderEngine>::hostage()
			->createDisplay(_width,_height,_bits,_fullScreen);
	}
}