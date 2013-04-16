#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#   include "Factory/FactoryPool.h"
#   include "ConstStringHolderPythonString.h"

#	include "pybind/pybind.hpp"

#	include <map>
#	include <list>
#	include <cstdarg>


namespace Menge
{
	class Entity;

	class ScriptLogger;
    class ScriptLoggerError;

    class ScriptModuleFinder;	

    class InputStreamInterface;

	class ScriptEngine
		: public ScriptServiceInterface
	{
	public:
		ScriptEngine();
		~ScriptEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		PyObject * initModule( const char * _name );

		PyObject * importModule( const ConstString& _name ) override;

		//PyObject * importPrototypeModule( const ResourceDesc & _resource ) override;
		PyObject * importPrototype( const ConstString& _name, bool & _exist );

		void setCurrentModule( PyObject * _module ) override;
        void addGlobalModule( const String & _name, PyObject * _module ) override;

        void addModulePath( const ConstString & _pak, const TVectorFilePath & _pathes ) override;

	public:
		static void incref( PyObject * _object );
		static void decref( PyObject * _object );
		static unsigned int refCount( PyObject * _obj );

	public:
		Entity * createEntity( const ConstString& _type, const ConstString & _prototype, PyObject * _generator ) override;

    public:
        ConstString stringize( PyObject * _str, bool & _valid ) override;

	public:
		void addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper ) override;
		PyObject * wrap( Node * _node ) override;
		
	public:
		bool hasModuleFunction( PyObject * _module, const char * _name );
		PyObject * getModuleFunction( PyObject * _module, const char * _name ) override;

		void callFunctionVA( PyObject * _object, const char * _params, va_list ) override;
		PyObject * askFunctionVA( PyObject * _object, const char * _params, va_list ) override;

		void callFunction( PyObject * _object, const char * _params, ...  ) override;
		PyObject * askFunction( PyObject * _object, const char * _params, ... ) override;

	public:
		static void handleException();
	
	private:
        ServiceProviderInterface * m_serviceProvider;

        ScriptModuleFinder * m_moduleFinder;

		PyObject * m_moduleMenge;

		ScriptLogger * m_loger;
		ScriptLoggerError * m_errorLogger;

		PyObject * m_internalObjectFinder;

		typedef std::map<ConstString, PyObject *> TMapModules;
		typedef std::map<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		TMapModules m_modules;

		typedef std::map<ConstString, ScriptClassInterface *> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

        typedef FactoryPool<ConstStringHolderPythonString, 256> FactoryPoolPythonString;
        FactoryPoolPythonString m_poolPythonString;
	};
}
