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

		void callModuleFunction( PyObject * _module, const std::string & _name );
		bool callModuleFunctionBool( PyObject * _module, const std::string & _name );
		
		void callModuleFunction( PyObject * _module, const std::string & _name, float f );

		void callFunction( PyObject * _object );
		void callFunction( const std::string & _name );
		
		void callFunction( PyObject * _object, float f );
		void callFunction( const std::string & _name, float f );
		void callFunctionNode( const std::string & _name, Node * _node );

		bool callFunctionBool( PyObject * _object );
		bool callFunctionBool( const std::string & _name );

		void callMethod( Entity * _entity, const std::string & _name );

		static void handleException();
	
	private:
		void doBuffer( const char * _buffer, size_t _size );
		

	private:
		ScriptObject * m_mainModule;
		ScriptGlobal * m_global;

		typedef std::list<std::string> TListEntitysType;
		TListEntitysType m_listEntitiesType;

		std::string m_pathEntities;
	};
}