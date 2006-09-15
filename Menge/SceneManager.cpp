#	include "SceneManager.h"
#	include "NodeFactory.h"

#	include "Node.h"

#	include "InputEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ScriptEngine.h"

#	include "ScriptFunction.h"

#	include "Manager/XmlManager.h"

#	include "windows.h"

using namespace RvEngine;


//////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
: m_inputEngine(0)
, m_scriptEngine(0)
, m_fileEngine(0)
, m_renderEngine(0)
//////////////////////////////////////////////////////////////////////////
, m_functionInit(0)
, m_functionUpdate(0)
, m_functionRender(0)
{
}
//////////////////////////////////////////////////////////////////////////
Node* SceneManager::createNode(const std::string &_name, const std::string &_type)
{
	Node *node = NodeFactory::genNode(_type,this);
	
	if( node )
	{
		node->setName(_name);
	}

	return node;
}
//////////////////////////////////////////////////////////////////////////
std::string SceneManager::getPath(const std::string &_type, const std::string &_value)
{
	TMapPaths::iterator it_find = m_mapPaths.find(_type);

	if( it_find == m_mapPaths.end() )
	{
		return _value;
	}

	std::string result = it_find->second;
	result += _value;

	return result;
}
//////////////////////////////////////////////////////////////////////////
void SceneManager::addPath(const std::string &_type, const std::string &_path)
{
	TMapPaths::iterator it_find = m_mapPaths.find(_type);

	if( it_find != m_mapPaths.end() )
	{
		return;
	}
	
	if( *_path.rbegin() != '/' )
	{
		std::string result = _path;
		result += "/";
		m_mapPaths.insert(std::make_pair(_type,result));
	}
	else
	{
		m_mapPaths.insert(std::make_pair(_type,_path));
	}
}
//////////////////////////////////////////////////////////////////////////
bool SceneManager::loadGameInfo(const std::string &_infoFile)
{
	XML_PARSE_FILE(_infoFile)
	{
		XML_CHECK_NODE_FOR_EACH("Game")
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
					m_fileEngine = new FileEngine(DllFile);					
				}

				XML_CHECK_NODE("InputSystem")
				{
					m_inputEngine = new InputEngine(DllFile);
				}
				
				XML_CHECK_NODE("RenderSystem")
				{
					m_renderEngine = new RenderEngine(DllFile);
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

					m_listLoadPaks
						.push_back(std::make_pair(file,priority));
				}

			}
		}
	}
	XML_INVALID_PARSE()
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool SceneManager::init(const std::string &_xmlFile)
{
	m_scriptEngine = new ScriptEngine();
	m_scriptEngine->init();

	RegisterScriptClass();

	if( loadGameInfo(_xmlFile) == false )
	{
		return false;
	}

	if( m_fileEngine == 0 ||
		m_renderEngine == 0 ||
		m_inputEngine == 0 ||
		m_scriptEngine == 0 )
	{
		return false;
	}

	for( TListLoadPaks::iterator
		it = m_listLoadPaks.begin(),
		it_end = m_listLoadPaks.end();
	it != it_end;
	++it)
	{
		m_fileEngine->loadPak(it->first,it->second);
	}

	XML_PARSE_FILE_EX(m_fileEngine, "app.xml")
	{
		//<paths>
		//	<path type = "Objects" dir = "Data/Objects" />
		//	<path type = "Scenes" dir = "Data/Scenes" />
		//	</paths>
		XML_CHECK_NODE_FOR_EACH("paths")
		{
			XML_CHECK_NODE("path")
			{
				XML_DEF_ATTRIBUTES_NODE(type);
				XML_DEF_ATTRIBUTES_NODE(dir);
				addPath(type,dir);
			}
		}

		//<script file = "logic/main.lua" >
		//	<init function = "init_game" />
		//	<update function = "update_game" />
		//	<render function = "render_game" />
		//	</script>
		XML_CHECK_NODE("script")
		{
			XML_DEF_ATTRIBUTES_NODE(file);

			m_scriptEngine->doFile(file);

			XML_FOR_EACH()
			{
				XML_CHECK_NODE("init")
				{
					XML_DEF_ATTRIBUTES_NODE(function);

					m_functionInit = new ScriptFunction(m_scriptEngine,function);
					m_functionInit->Accept();
				}

				XML_CHECK_NODE("update")
				{
					XML_DEF_ATTRIBUTES_NODE(function);

					m_functionUpdate = new ScriptFunction(m_scriptEngine,function);
					m_functionUpdate->Accept();
				}

				XML_CHECK_NODE("render")
				{
					XML_DEF_ATTRIBUTES_NODE(function);

					m_functionRender = new ScriptFunction(m_scriptEngine,function);
					m_functionRender->Accept();
				}

			}
		}

		//<chapter name = "Test" scene = "Buka" />
		XML_CHECK_NODE("chapter")
		{
			XML_DEF_ATTRIBUTES_NODE(name);
			XML_DEF_ATTRIBUTES_NODE(scene);

			
		}
	}

	m_renderEngine->init(m_fileEngine);

	if( m_functionInit )
	{
		return m_functionInit->callFunctionBool();
	}

	return true;
}

#	include "Scene.h"
#	include "Layer.h"
#	include "Track.h"
#	include "Sprite.h"


static Scene *g_scene;

void init_base_scene(SceneManager *sceneMgr)
{
	g_scene = sceneMgr->createNodeT<Scene>("Scene1","Scene");

	Layer *layer = g_scene->createLayer("Base");

	Track *track = sceneMgr->createNodeT<Track>("Track1","Track");
	layer->addChildren(track);

	track->addPoint(mt::vec2f(0,0));
	track->addPoint(mt::vec2f(100,100));
	track->addPoint(mt::vec2f(200,50));
	track->addPoint(mt::vec2f(300,150));

	Sprite *sprite1 = sceneMgr->createNodeT<Sprite>("Sprite1","Sprite");
	track->addChildren(sprite1);

	Sprite *sprite2 = sceneMgr->createNodeT<Sprite>("Sprite2","Sprite");
	sprite1->addChildren(sprite2);
}

//////////////////////////////////////////////////////////////////////////
void SceneManager::run()
{	
	init_base_scene(this);

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
void SceneManager::update()
{
	if( m_functionUpdate )
	{
		m_functionUpdate->callFunctionVoid();
	}

	g_scene->update(0.1f);
}
//////////////////////////////////////////////////////////////////////////
void SceneManager::render()
{
	m_renderEngine->beginSceneDrawing(true,true,0x00000000);

	if( m_functionRender )
	{
		m_functionRender->callFunctionVoid();
	}

	m_renderEngine->drawLine(mt::vec2f(0.f,0.f),mt::vec2f(400.f,300.f),1.f,0xffffffff);

	g_scene->debugRender();

	m_renderEngine->endSceneDrawing();
}
//////////////////////////////////////////////////////////////////////////
bool SceneManager::createDisplay(
				   unsigned int _width, 
				   unsigned int _height, 
				   unsigned int _bits, 
				   bool _fullScreen)
{
	return m_renderEngine->createDisplay(_width,_height,_bits,_fullScreen);
}
//////////////////////////////////////////////////////////////////////////
ScriptEngine * SceneManager::getScriptEngine()
{
	return m_scriptEngine;
}
//////////////////////////////////////////////////////////////////////////
FileEngine * SceneManager::getFileEngine()
{
	return m_fileEngine;
}
//////////////////////////////////////////////////////////////////////////
InputEngine * SceneManager::getInputEngine()
{
	return m_inputEngine;
}
//////////////////////////////////////////////////////////////////////////
RenderEngine * SceneManager::getRenderEngine()
{
	return m_renderEngine;
}