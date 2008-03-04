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

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		typedef std::list< std::string > TListModulePath;

	public:
		void init();

		bool doFile( const std::string & _file );
		bool doString( const std::string & _string );

		PyObject * initModule( const std::string & _file );
		PyObject * importModule( const std::string & _file );

		void setCurrentModule( PyObject * _module );
		
		void setModulePath( const TListModulePath & _listPath );

		bool isEntityType( const std::string & _type );		
		PyObject * getEntityModule( const std::string & _type );
		bool registerEntityType( const std::string & _type );
		FileDataInterface * getEntityXML( const std::string & _type );

		static PyObject * wrap( Node * _node );
		static PyObject * proxy( PyObject * _module, const std::string & _name, void * _impl );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static size_t refCount( PyObject * _obj );

	public:
		PyObject * genEvent( const std::string &_name );

		Entity * createEntity( const std::string & _type );
		Arrow * createArrow( const std::string & _type );
		Scene * createScene( const std::string & _type );
		
		bool hasModuleFunction( PyObject * _module, const std::string & _name );
		PyObject * getModuleFunction( PyObject * _module, const std::string & _name );
		PyObject * callModuleFunction( const std::string & _module, const std::string & _name, const char * _params = "()", ... );
		PyObject * callModuleFunction( PyObject * _module, const std::string & _name, const char * _params = "()", ... );

		PyObject * callFunction( PyObject * _object, const char * _params, va_list );

		PyObject * callFunction( PyObject * _object, const char * _params = "()", ...  );
		PyObject * callFunction( const std::string & _name, const char * _params = "()", ...  );

		bool hasMethod( Node * _entity, const std::string & _name );
		PyObject * callMethod( Node * _entity, const std::string & _name, const char * _params = "()", ...  );

		bool parseBool( PyObject * _result );
		
		static void handleException();
	
	private:
		void doBuffer( const char * _buffer, size_t _size );
		

	private:
		PyObject * m_global;

		typedef std::map<std::string, PyObject *> TMapEntitiesType;
		TMapEntitiesType m_mapEntitiesType;

		typedef std::map<std::string, FileDataInterface *> TMapEntitiesXML;
		TMapEntitiesXML m_mapEntitiesXML;

		typedef std::map<std::string, PyObject *> TMapModule;
		TMapModule m_mapModule;
	};
}