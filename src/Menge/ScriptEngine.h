#	pragma once

#	include "Holder.h"

#	include <map>
#	include <list>
#	include "FileEngine.h"

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
	class Arrow;
	class Scene;

	class ScriptLogger
	{
	public:
		void write( const std::string & _msg );
	};

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		typedef std::list< std::string > TListModulePath;

	public:
		void init();

		bool compileString( const std::string & _string, const std::string & _file );

		bool doFile( const std::string & _file );
		bool doString( const std::string & _string );

		PyObject * initModule( const String& _file );
		PyObject * importModule( const String& _file );

		void setCurrentModule( PyObject * _module );
		
		void setModulePath( const TListModulePath & _listPath );

		bool isEntityType( const String& _type );		
		PyObject * getEntityModule( const String& _type );
		bool registerEntityType( const String& _type );
		TVectorChar * getEntityXML( const String& _type );

		static PyObject * wrap( Node * _node );
		static PyObject * proxy( PyObject * _module, const std::string & _name, void * _impl );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		PyObject * genEvent( const std::string &_name );

		Entity * createEntity( const String& _type );
		Arrow * createArrow( const String& _type );
		Scene * createScene( const String& _type );
		
		bool hasModuleFunction( PyObject * _module, const String& _name );
		PyObject * getModuleFunction( PyObject * _module, const String& _name );
		PyObject * callModuleFunction( const String& _module, const String& _name, const char * _params = "()", ... );
		PyObject * callModuleFunction( PyObject * _module, const String& _name, const char * _params = "()", ... );

		PyObject * callFunction( PyObject * _object, const char * _params, va_list );

		PyObject * callFunction( PyObject * _object, const char * _params = "()", ...  );
		PyObject * callFunction( const std::string & _name, const char * _params = "()", ...  );

		bool hasMethod( Node * _entity, const std::string & _name );
		PyObject * callMethod( Node * _entity, const std::string & _name, const char * _params = "()", ...  );

		bool parseBool( PyObject * _result );
		
		static void handleException();
	
	private:
		void doBuffer( const char * _buffer, unsigned int _size );
		

	private:
		PyObject * m_global;

		ScriptLogger m_loger;

		typedef std::map<String, PyObject *> TMapEntitiesType;
		TMapEntitiesType m_mapEntitiesType;

		typedef std::map<String, TVectorChar > TMapEntitiesXML;
		TMapEntitiesXML m_mapEntitiesXML;

		typedef std::map<String, PyObject *> TMapModule;
		TMapModule m_mapModule;
	};
}