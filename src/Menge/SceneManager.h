#	pragma once

#	include "SceneManagerExport.h"

#	include <string>
#	include <map>
#	include <list>

class RenderImageInterface;

namespace RvEngine
{
	class Node;
	class Scene;	

	class InputEngine;
	class ScriptEngine;
	class FileEngine;
	class RenderEngine;

	class ScriptFunction;

	class SCENE_MANAGER_API SceneManager
	{
	public:
		SceneManager();

	public:
		bool init(const std::string &_xmlFile);

		bool createDisplay(
			unsigned int _width, 
			unsigned int _height, 
			unsigned int _bits, 
			bool _fullScreen);

		void run();

		void update();
		void render();

	public:
		Node * createNode(const std::string &_name, const std::string &_type);

		template<class T>
		T* createNodeT(const std::string &_name, const std::string &_type)
		{
			return static_cast<T*>(createNode(_name,_type));
		}


		std::string getPath(const std::string &_type,const std::string &_value);

	public:
		ScriptEngine * getScriptEngine();
		FileEngine * getFileEngine();
		InputEngine * getInputEngine();
		RenderEngine * getRenderEngine();

	private:
		void RegisterScriptClass();

		bool loadGameInfo(const std::string &_infoFile);

		void addPath(const std::string &_type, const std::string &_path);

	private:
		InputEngine * m_inputEngine;
		ScriptEngine * m_scriptEngine;
		FileEngine * m_fileEngine;
		RenderEngine * m_renderEngine;

		ScriptFunction * m_functionInit;
		ScriptFunction * m_functionUpdate;
		ScriptFunction * m_functionRender;

		typedef std::map<std::string,std::string> TMapPaths;
		
		TMapPaths m_mapPaths;

		typedef std::list< std::pair<std::string,int> > TListLoadPaks;
		TListLoadPaks m_listLoadPaks;
	};
}