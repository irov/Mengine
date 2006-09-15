#	pragma once

#	include <string>
#	include <set>

class CLuaScript;

namespace RvEngine
{
	class ScriptFunction;

	class Node;

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		void init();

		CLuaScript* getLuaScript();

		void include(const std::string &_file);
		int doFile(const std::string &_file);
		int doString(const std::string &_string);

		void setLuaObject(const std::string &_name, Node *_object );

		ScriptFunction * genEvent(const std::string &_name);

	public:
		template<class T>
		T callFunction(const std::string &_function, Node *_object);

		void callFunctionVoid(const std::string &_function, Node *_object);
		int callFunctionInt(const std::string &_function, Node *_object);

		template<class T>
		T callFunction(const std::string &_function);

		void callFunctionVoid(const std::string &_function);
		int callFunctionInt(const std::string &_function);

	public:
		const std::string & getErrorString();

	private:
		typedef std::set<std::string> TSetLuaFile;

		CLuaScript*	m_luaScript;

		TSetLuaFile m_setLuaFile;
		TSetLuaFile m_queueLuaFile;
	};
}