#	pragma once

#	include "Interface/ScriptSystemInterface.h"

#	include "ScriptClassInterface.h"

#	include "FileEngine.h"

#	include "Core/Holder.h"

#	include "pybind/pybind.hpp"

#	include <map>
#	include <list>
#	include <cstdarg>


namespace Menge
{
	class Entity;

	class ScriptLogger;
	class ErrorScriptLogger;

	class ScriptEngine
		: public Holder<ScriptEngine>
		, public ScriptServiceInterface
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		void initialize();
		void finalize();

	public:
		PyObject * initModule( const char * _name );

		PyObject * importModule( const ConstString& _name ) override;
		PyObject * importPrototype( const ConstString& _name, const ConstString & _category, const ConstString & _pak, const WString & _path, bool & _exist );

		void setCurrentModule( PyObject * _module ) override;
		
		void addModulePath( const TVectorWString& _listPath ) override;

	public:
		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	protected:
		void updateModulePath_();

	public:
		Entity * createEntity( const ConstString & _name, const ConstString& _type, const ConstString& _tag, const ConstString& _prototype, const ConstString & _pak, const WString & _path );

		template<class T>
		T * createEntityT( const ConstString & _name, const ConstString& _type, const ConstString& _tag, const ConstString& _prototype, const ConstString & _pak, const WString & _path )
		{
			Entity * entity = this->createEntity( _name, _type, _tag, _prototype, _pak, _path );

			if( dynamic_cast<T*>(entity) == NULL )
			{
				return NULL;
			}

			T * t = static_cast<T*>(entity);

			return t;
		}

	public:
		void addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper );
		PyObject * wrap( Node * _node );
		
	public:
		bool hasModuleFunction( PyObject * _module, const char * _name );
		PyObject * getModuleFunction( PyObject * _module, const char * _name ) override;

		void callFunction( PyObject * _object, const char * _params, va_list ) override;
		PyObject * askFunction( PyObject * _object, const char * _params, va_list );

		void callFunction( PyObject * _object, const char * _params, ...  ) override;
		PyObject * askFunction( PyObject * _object, const char * _params, ... );
		
	public:
		static void handleException();
	
	private:
		PyObject * m_moduleMenge;

		ScriptLogger * m_loger;
		ErrorScriptLogger * m_errorLogger;

		PyObject * m_internalObjectFinder;

		typedef std::map<ConstString, PyObject *> TMapModules;
		typedef std::map<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		TMapModules m_modules;

		typedef std::map<ConstString, ScriptClassInterface *> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

		TVectorWString m_modulePaths;
	};
}
