#pragma once

#include "Interface/ScriptSystemInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/PrototypeManagerInterface.h"

#include "ConstStringHolderPythonString.h"

#include "Core/ServiceBase.h"

#include "ScriptLogger.h"
#include "ScriptModule.h"
#include "ScriptModuleFinder.h"

#include "Kernel/Entity.h"

#include "Factory/Factory.h"

#include "pybind/pybind.hpp"

#include "stdex/stl_map.h"

#include <cstdarg>

namespace Mengine
{
	class ScriptEngine
		: public ServiceBase<ScriptServiceInterface>
	{
	public:
		ScriptEngine();
		~ScriptEngine() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	protected:
		pybind::kernel_interface * getKernel() override;

	public:
		PyObject * initModule( const char * _name );

		ScriptModuleInterfacePtr importModule( const ConstString& _name ) override;

		void setCurrentModule( PyObject * _module ) override;
        
		void addGlobalModule( const Char * _name, PyObject * _module ) override;
		void removeGlobalModule( const Char * _name ) override;

	public:
        void addModulePath( const ConstString & _pak, const TVectorScriptModulePack & _modules ) override;
		void removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) override;

	public:
		bool bootstrapModules() override;
		bool initializeModules() override;
		bool finalizeModules() override;

	protected:
		bool initializeModule_( const ScriptModulePack & _pack );
		
    public:
        bool stringize( PyObject * _object, ConstString & _str ) override;

	public:
		void setWrapper( const ConstString& _type, const ScriptWrapperInterfacePtr & _wrapper ) override;
        void removeWrapper(const ConstString& _type) override;
        const ScriptWrapperInterfacePtr & getWrapper( const ConstString & _type ) const override;
		
	public:
		PyObject * loadModuleSource( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) override;
		PyObject * loadModuleBinary( PyObject * _moduleName, bool _packagePath, const MemoryInterfacePtr & _stream ) override;

	public:
		static void handleException();
	
	private:
		pybind::kernel_interface * m_kernel;

		ScriptModuleFinderPtr m_moduleFinder;

		PyObject * m_moduleMenge;

		TVectorScriptModulePack m_bootstrapperModules;

		ScriptLogger * m_loggerWarning;
		ScriptLogger * m_loggerError;

		typedef stdex::map<ConstString, PyObject *> TMapModules;
		typedef stdex::map<ConstString, TMapModules> TMapCategoryPrototypies;
		TMapCategoryPrototypies m_prototypies;

		typedef stdex::map<ConstString, ScriptWrapperInterfacePtr> TMapScriptWrapper;
		TMapScriptWrapper m_scriptWrapper;

        FactoryPtr m_factoryPythonString;		
		FactoryPtr m_factoryScriptModule;
		
		bool m_initializeModules;
	};
}
