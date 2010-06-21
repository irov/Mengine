#	pragma once

#	include "Core/Holder.h"

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

	class FactoryIdentity;

	class ScriptLogger
	{
	public:
		ScriptLogger();

	public:
		void write( const String& _msg );

	public:
		void setSoftspace( int _softspace );
		int getSoftspace() const;

	protected:
		int m_softspace;
	};

	class ErrorScriptLogger
		: public ScriptLogger
	{
	public:
		void write( const String& _msg );
	};

	class ScriptEngine
		: public Holder<ScriptEngine>
	{
	public:
		ScriptEngine( FactoryIdentity * _factoryIdentity );
		~ScriptEngine();

		typedef std::list<String> TListModulePath;

	public:
		void initialize();
		void exec( const String& _command );

		PyObject * initModule( const char * _name );

		PyObject * importModule( const ConstString& _name, const ConstString& _type );

		void setCurrentModule( PyObject * _module );
		
		void addModulePath( const TListModulePath& _listPath );

		bool isEntityType( const ConstString& _type );		
		PyObject * getEntityPyType( const ConstString& _type );
		bool registerEntityType( const ConstString& _packName, const String& _path, const ConstString& _type );
		Blobject * getEntityXML( const ConstString& _type );

		static PyObject * wrap( Node * _node );
		static PyObject * proxy( PyObject * _module, const char * _name, void * _impl );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		Entity * createEntity( const ConstString& _type );
		Entity * createEntityFromXml( const ConstString& _type, const String& _xml );

		Arrow * createArrow( const ConstString& _type );
		Scene * createScene( const ConstString& _type );
		
		bool hasModuleFunction( PyObject * _module, const char * _name );
		PyObject * getModuleFunction( PyObject * _module, const char * _name );
		void callModuleFunction( const ConstString& _module, const char * _name, const char * _params, ... );

		void callModuleFunction( PyObject * _module, const char * _name, const char * _params, ... );
		PyObject * askModuleFunction( PyObject * _module, const char * _name, const char * _params, ... );

		void callFunction( PyObject * _object, const char * _params, va_list );
		PyObject * askFunction( PyObject * _object, const char * _params, va_list );

		void callFunction( PyObject * _object, const char * _params, ...  );
		PyObject * askFunction( PyObject * _object, const char * _params, ... );

		bool hasMethod( Node * _entity, const char * _name );
		void callMethod( Node * _entity, const char * _name, const char * _params, ...  );

		bool parseBool( PyObject * _result );
		
		static void handleException();
	
	public:
		void writeError( const std::string & _message );

	private:
		Entity * createEntity_( const ConstString& _type );
		Entity * createEntityFromXml_( const ConstString& _type, const void * _buffer, std::size_t _size );

	private:
		PyObject * m_global;

		ScriptLogger m_loger;
		ErrorScriptLogger m_errorLogger;

		typedef std::map<ConstString, PyObject *> TMapEntitiesType;
		TMapEntitiesType m_mapEntitiesType;

		typedef std::map<ConstString, Blobject> TMapEntitiesXML;
		TMapEntitiesXML m_mapEntitiesXML;

		typedef std::map<ConstString, PyObject *> TMapModule;
		TMapModule m_mapModule;

		TListModulePath m_modulePaths;

		FactoryIdentity * m_factoryIdentity;
	};
}
