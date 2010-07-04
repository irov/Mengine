#	pragma once

#	include "FileEngine.h"

#	include "Core/Holder.h"

#	include <map>
#	include <list>
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
		ScriptEngine();
		~ScriptEngine();

		typedef std::list<String> TListModulePath;

	public:
		void initialize();

		PyObject * initModule( const char * _name );
		PyObject * importModule( const ConstString& _name, const ConstString& _type, const ConstString& _class );

		void setCurrentModule( PyObject * _module );
		
		void addModulePath( const TListModulePath& _listPath );

		static PyObject * wrap( Node * _node );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		Node * createNode( const ConstString& _type, const ConstString& _category, const ConstString& _class );

		template<class T>
		T * createNodeT( const ConstString& _type, const ConstString& _category, const ConstString& _class )
		{
			return static_cast<T*>( this->createNode( _type, _category, _class ) );
		}
		
	public:
		bool hasModuleFunction( PyObject * _module, const char * _name );
		PyObject * getModuleFunction( PyObject * _module, const char * _name );

		void callFunction( PyObject * _object, const char * _params, va_list );
		PyObject * askFunction( PyObject * _object, const char * _params, va_list );

		void callFunction( PyObject * _object, const char * _params, ...  );
		PyObject * askFunction( PyObject * _object, const char * _params, ... );
		
	public:
		static void handleException();
	
	private:
		PyObject * m_global;

		ScriptLogger m_loger;
		ErrorScriptLogger m_errorLogger;

		typedef std::map<ConstString, PyObject *> TMapModule;
		TMapModule m_modules;

		TListModulePath m_modulePaths;
	};
}
