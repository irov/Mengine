#	pragma once

#	include "Holder.h"

#	include <map>
#	include <list>

namespace Menge
{
	class Node;
	class Scriptable;
	class Entity;

	class ScriptObject;
	class ScriptGlobal;

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		void init();

		void export_function();

		bool doFile( const std::string & _file );
		bool doString( const std::string & _string );

	public:
		ScriptObject * genFunctor( const std::string &_name );
		void removeFunctor( ScriptObject * _functor );

		Entity * createEntity( const std::string & _type, const std::string & _name );

		void callFunction( const std::string & _name, const char * _format = "()" , ... );

		bool callFunctionBool( const std::string & _name, const char * _format = "()" , ... );
	
	private:
		void doBuffer( const char * _buffer, size_t _size );
		void handleException();

	private:
		ScriptObject * m_mainModule;
		ScriptGlobal * m_global;
	};
}