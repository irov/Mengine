#	pragma once

#	include "ScriptClassInterface.h"

#	include "ScriptLogger.h"

#	include "FileEngine.h"

#	include "Core/Holder.h"

#	include "pybind/pybind.hpp"

#	include <map>
#	include <list>
#	include <cstdarg>


namespace Menge
{
	class Node;

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

		PyObject * importModule( const ConstString& _name );
		PyObject * importPrototype( const ConstString& _name, const ConstString & _category, const ConstString & _pak, const ConstString & _path );

		void setCurrentModule( PyObject * _module );
		
		void addModulePath( const TListModulePath& _listPath );

		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		Node * createNode( const ConstString& _prototype, const ConstString& _category, const ConstString & _pak, const ConstString & _path );

		template<class T>
		T * createNodeT( const ConstString& _prototype, const ConstString& _category, const ConstString & _pak, const ConstString & _path )
		{
			return static_cast<T*>( this->createNode( _prototype, _category, _pak, _path ) );
		}

	public:
		void regWrapping( const ConstString& _type, ScriptClassInterface * _wrapper );
		PyObject * wrap( Node * _node );
		
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

		typedef std::map<ConstString, PyObject *> TMapModules;
		typedef std::map<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		TMapModules m_modules;

		typedef std::map<ConstString, ScriptClassInterface *> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

		TListModulePath m_modulePaths;
	};
}
