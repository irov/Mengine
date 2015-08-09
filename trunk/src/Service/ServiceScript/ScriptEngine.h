#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#	include "EntityPrototypeGenerator.h"
#   include "ConstStringHolderPythonString.h"

#   include "Factory/FactoryStore.h"
#   include "Factory/FactoryDefault.h"

#	include "pybind/pybind.hpp"

#   include "stdex/binary_vector.h"

#	include <list>
#	include <cstdarg>

namespace Menge
{
	class Entity;

	class ScriptLogger;
    class ScriptLoggerError;

    class ScriptModuleFinder;	

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

		pybind::object importModule( const ConstString& _name ) override;

		void setCurrentModule( PyObject * _module ) override;
        
        void addGlobalModule( const String & _name, PyObject * _module ) override;
        void removeGlobalModule( const String & _name ) override;

        void addModulePath( const ConstString & _pak, const TVectorScriptModulePak & _modules ) override;

	public:
		bool bootstrapModules() override;
		bool initializeModules() override;

	public:
		PrototypeGeneratorInterfacePtr createEntityGenerator( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator ) override;
		pybind::object importEntity( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		Entity * createEntity( const ConstString& _type, const ConstString & _prototype, const pybind::object & _generator, Eventable * _eventable ) override;

    public:
        bool stringize( PyObject * _object, ConstString & _str ) override;

	public:
		void addWrapping( const ConstString& _type, ScriptClassInterface * _wrapper ) override;
		PyObject * wrap( const ConstString & _type, Scriptable * _node ) override;
		
	public:
		bool hasModuleFunction( const pybind::object & _module, const char * _name ) override;
		pybind::object getModuleFunction( const pybind::object & _module, const char * _name ) override;

	public:
		static void handleException();
	
	private:
        ServiceProviderInterface * m_serviceProvider;

        ScriptModuleFinder * m_moduleFinder;

		PyObject * m_moduleMenge;

		TVectorScriptModulePak m_bootstrapperModules;

		ScriptLogger * m_logger;
		ScriptLoggerError * m_loggerError;

		typedef stdex::binary_vector<ConstString, PyObject *> TMapModules;
		typedef stdex::binary_vector<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		typedef stdex::binary_vector<ConstString, ScriptClassInterface *> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

        typedef FactoryPoolStore<ConstStringHolderPythonString, 256> FactoryConstStringHolderPythonString;
        FactoryConstStringHolderPythonString m_factoryPythonString;			

		typedef FactoryPoolStore<EntityPrototypeGenerator, 64> FactoryEntityPrototypeGenerator;
		FactoryEntityPrototypeGenerator m_factoryEntityPrototypeGenerator;
	};
}
