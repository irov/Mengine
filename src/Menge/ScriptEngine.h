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

		bool doFile( const std::string & _file );
		bool doString( const std::string & _string );

		void setEntitiesPath( const std::string & _path );
		bool isEntityType( const std::string & _type );
		void registerEntityType( const std::string & _type );

		void incref( Node * _node );
		void decref( Node * _node );

	public:
		ScriptObject * genFunctor( const std::string &_name );
		void removeFunctor( ScriptObject * _functor );

		Entity * createEntity( const std::string & _type );
		void removeEntity( Entity * _entity );

		void callFunction( const std::string & _name, const char * _format = "()" , ... );
		void callFunctionNode( const std::string & _name, Node * _node );

		bool callFunctionBool( const std::string & _name, const char * _format = "()" , ... );

		void callMethod( Entity * _entity, const std::string & _name, const char * _format = "()" , ... );

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