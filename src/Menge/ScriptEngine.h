#	pragma once

#	include "Holder.h"

#	include <map>
#	include <list>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

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

		bool doFile( const std::string & _file );
		bool doString( const std::string & _string );

		PyObject * importModule( const std::string & _file );

		void setEntitiesPath( const std::string & _paths );
		bool isEntityType( const std::string & _type );
		void registerEntityType( const std::string & _type );

		static void incref( Node * _node );
		static void decref( Node * _node );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );

	public:
		PyObject * genFunctor( const std::string &_name );

		Entity * createEntity( const std::string & _type );

		bool hasModuleFunction( PyObject * _module, const std::string & _name );
		PyObject * callModuleFunction( PyObject * _module, const std::string & _name, const char * _params = "()", ... );

		PyObject * callFunction( PyObject * _object, const char * _params = "()", ...  );
		PyObject * callFunction( const std::string & _name, const char * _params = "()", ...  );

		bool hasMethod( Node * _entity, const std::string & _name );
		PyObject * callMethod( Node * _entity, const std::string & _name, const char * _params = "()", ...  );

		bool parseBool( PyObject * _result );

		static void handleException();
	
	private:
		void doBuffer( const char * _buffer, size_t _size );
		

	private:
		ScriptObject * m_globalObject;
		ScriptGlobal * m_globalDict;

		typedef std::list<std::string> TListEntitysType;
		TListEntitysType m_listEntitiesType;

		std::string m_pathEntities;
	};
}