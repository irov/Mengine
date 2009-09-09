#	pragma once

#	include "Holder.h"

#	include <map>
#	include <list>
#	include "FileEngine.h"
#	include <cstdarg>
#	include "pybind/pybind.hpp"

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
		void write( const String& _msg );
	};

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		typedef std::list< String > TListModulePath;

	public:
		void init();
		void exec( const String& _command );

		PyObject * initModule( const String& _file );
		PyObject * importModule( const String& _file );

		void setCurrentModule( PyObject * _module );
		
		void setModulePath( const TListModulePath & _listPath );
		void addModulePath( const TListModulePath& _listPath );

		bool isEntityType( const String& _type );		
		PyObject * getEntityModule( const String& _type );
		bool registerEntityType( const String& _packName, const String& _path, const String& _type );
		Blobject * getEntityXML( const String& _type );

		static PyObject * wrap( Node * _node );
		static PyObject * proxy( PyObject * _module, const String & _name, void * _impl );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		PyObject * genEvent( const String &_name );

		Entity * createEntity( const String& _type );
		Entity * createEntityWithXml( const String& _type, const String& _xml );
		Arrow * createArrow( const String& _type );
		Scene * createScene( const String& _type );
		
		bool hasModuleFunction( PyObject * _module, const String& _name );
		PyObject * getModuleFunction( PyObject * _module, const String& _name );
		void callModuleFunction( const String& _module, const String& _name, const char * _params, ... );

		void callModuleFunction( PyObject * _module, const String& _name, const char * _params, ... );
		PyObject * askModuleFunction( PyObject * _module, const String& _name, const char * _params, ... );

		void callFunction( PyObject * _object, const char * _params, va_list );
		PyObject * askFunction( PyObject * _object, const char * _params, va_list );

		void callFunction( PyObject * _object, const char * _params, ...  );
		PyObject * askFunction( PyObject * _object, const char * _params, ... );

		bool hasMethod( Node * _entity, const String & _name );
		void callMethod( Node * _entity, const String & _name, const char * _params, ...  );



		bool parseBool( PyObject * _result );
		
		static void handleException();
	
	private:
		Entity * createEntity_( const String& _type );

	private:
		PyObject * m_global;

		ScriptLogger m_loger;

		typedef std::map<String, PyObject *> TMapEntitiesType;
		TMapEntitiesType m_mapEntitiesType;

		typedef std::map<String, Blobject > TMapEntitiesXML;
		TMapEntitiesXML m_mapEntitiesXML;

		typedef std::map<String, PyObject *> TMapModule;
		TMapModule m_mapModule;

		typedef std::map< String, String > TEntityPackMap;
		TEntityPackMap m_entityPackMap;

		TListModulePath m_modulePaths;
	};
}
