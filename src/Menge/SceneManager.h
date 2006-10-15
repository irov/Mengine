#	pragma once

#	include "Keeper.h"

#	include <string>
#	include <map>
#	include <list>

class RenderImageInterface;

namespace Menge
{
	class Node;
	class Scene;	
	class Chapter;

	class InputEngine;
	class ScriptEngine;
	class FileEngine;
	class RenderEngine;

	class ScriptFunction;
	

	class SceneManager
	{
	public:
		SceneManager();

	public:
		Node * createNode(const std::string &_name, const std::string &_type);

		template<class T>
		T* createNodeT(const std::string &_name, const std::string &_type)
		{
			return static_cast<T*>(createNode(_name,_type));
		}

		bool loadNode(Node *_node, const std::string &_xml);

	private:
		void RegisterScriptClass();

		typedef std::list< std::pair<std::string,int> > TListLoadPaks;
		TListLoadPaks m_listLoadPaks;
	};
}