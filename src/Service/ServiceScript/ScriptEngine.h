#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#   include "ConstStringHolderPythonString.h"

#	include "ScriptLogger.h"
#	include "ScriptModule.h"
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

	protected:
		pybind::kernel_interface * getKernel() override;

	public:
		PyObject * initModule( const char * _name );

		ScriptModuleInterfacePtr importModule( const ConstString& _name ) override;

		void setCurrentModule( ScriptObject * _module ) override;
        
		void addGlobalModule( const Char * _name, ScriptObject * _module ) override;
		void removeGlobalModule( const Char * _name ) override;

	public:
        void addModulePath( const ConstString & _pak, const TVectorScriptModulePack & _modules ) override;
		void removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) override;

	public:
		bool bootstrapModules() override;
		bool initializeModules() override;
		bool finalizeModules() override;
		
    public:
        bool stringize( ScriptObject * _object, ConstString & _str ) override;

	public:
		void setWrapper( const ConstString& _type, ScriptWrapperInterface * _wrapper ) override;
		ScriptWrapperInterface * getWrapper( const ConstString & _type ) const override;
		
	public:
		static void handleException();
	
	private:
		pybind::kernel_interface * m_kernel;

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

		typedef FactoryPoolStore<ScriptModule, 8> FactoryScriptModuleInterface;
		FactoryScriptModuleInterface m_factoryScriptModule;
		
		bool m_initializeModules;
	};
}
