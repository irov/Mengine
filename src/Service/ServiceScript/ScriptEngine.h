#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#	include "EntityPrototypeGenerator.h"
#   include "ConstStringHolderPythonString.h"

#	include "ScriptLogger.h"
#	include "ScriptModuleFinder.h"

#	include "Kernel/Entity.h"

#   include "Factory/FactoryStore.h"
#   include "Factory/FactoryDefault.h"

#	include "pybind/pybind.hpp"

#   include "stdex/stl_map.h"

#	include <list>
#	include <cstdarg>

namespace Menge
{
	class ScriptEngine
		: public ServiceBase<ScriptServiceInterface>
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		PyObject * initModule( const char * _name );

		pybind::object importModule( const ConstString& _name ) override;

		void setCurrentModule( PyObject * _module ) override;
        
        void addGlobalModule( const String & _name, PyObject * _module ) override;
        void removeGlobalModule( const String & _name ) override;

	public:
        void addModulePath( const ConstString & _pak, const TVectorScriptModulePack & _modules ) override;
		void removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) override;

	public:
		bool bootstrapModules() override;
		bool initializeModules() override;
		bool finalizeModules() override;

	public:
		bool addEntityPrototype( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator ) override;
		pybind::object importEntity( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		Entity * createEntity( const ConstString& _type, const ConstString & _prototype, const pybind::object & _generator, Eventable * _eventable ) override;

    public:
        bool stringize( PyObject * _object, ConstString & _str ) override;

	public:
		void setWrapper( const ConstString& _type, ScriptWrapperInterface * _wrapper ) override;
		ScriptWrapperInterface * getWrapper( const ConstString & _type ) const override;
		
	public:
		static void handleException();
	
	private:
        ScriptModuleFinder * m_moduleFinder;

		PyObject * m_moduleMenge;

		TVectorScriptModulePack m_bootstrapperModules;

		ScriptLogger * m_loggerWarning;
		ScriptLogger * m_loggerError;

		typedef stdex::map<ConstString, PyObject *> TMapModules;
		typedef stdex::map<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		typedef stdex::map<ConstString, ScriptWrapperInterface *> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

        typedef FactoryPoolStore<ConstStringHolderPythonString, 256> FactoryConstStringHolderPythonString;
        FactoryConstStringHolderPythonString m_factoryPythonString;			

		typedef FactoryPoolStore<EntityPrototypeGenerator, 64> FactoryEntityPrototypeGenerator;
		FactoryEntityPrototypeGenerator m_factoryEntityPrototypeGenerator;

		bool m_initializeModules;
	};
}
