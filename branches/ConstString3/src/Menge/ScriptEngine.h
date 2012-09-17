#	pragma once

#	include "Interface/ScriptSystemInterface.h"

#	include "ScriptClassInterface.h"

#	include "FileEngine.h"
#	include "ResourceManager.h"

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

		//PyObject * importPrototypeModule( const ResourceDesc & _resource ) override;
		PyObject * importPrototype( const ConstString& _name, bool & _exist );

		void setCurrentModule( PyObject * _module ) override;
        void addGlobalModule( const String & _name, PyObject * _module ) override;

		void addModulePath( const TVectorWString& _listPath ) override;

	public:
		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	protected:
		void updateModulePath_();

	public:
		Entity * createEntity( const ConstString& _type, PyObject * _prototype );

		template<class T>
		T * createEntityT( const ConstString& _type, PyObject * _prototype )
		{
			Entity * entity = this->createEntity( _type, _prototype );

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
