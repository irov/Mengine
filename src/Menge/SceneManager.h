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

		Node * createNodeFromXml( const std::string &_file);

		template<class T>
		T * createNodeFromXmlT( const std::string &_file)
		{
			return static_cast<T*>(createNodeFromXml(_file));
		}

		bool loadNode(Node *_node, const std::string &_xml);

	private:
		void RegisterScriptClass();
	};
}