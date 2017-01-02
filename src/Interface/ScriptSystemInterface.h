#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/PrototypeManagerInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/Tags.h"

#   include "pybind/base.hpp"

#	include <stdex/stl_vector.h>

#	include <stdarg.h>

namespace Menge
{	    
    class Scriptable;
	class Eventable;
    //////////////////////////////////////////////////////////////////////////
    typedef void * ScriptObject;
	//////////////////////////////////////////////////////////////////////////
    class ScriptWrapperInterface
    {
	public:
		ScriptWrapperInterface(){};
		virtual ~ScriptWrapperInterface(){};

	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() = 0;

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

    public:
		virtual ScriptObject * wrap( Scriptable * _node ) = 0;

    public:
        virtual void destroy() = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	struct ScriptModulePack
	{
		FilePath path;
		ConstString module;
		ConstString initializer;
		ConstString finalizer;
		Tags platform;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ScriptModulePack> TVectorScriptModulePack;
	//////////////////////////////////////////////////////////////////////////
	class ScriptModuleInterface
		: public FactorablePtr
	{
	public:
		virtual bool onInitialize( const ConstString & _method ) = 0;
		virtual bool onFinalize( const ConstString & _method ) = 0;

    public:
        virtual const pybind::base & getModule() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ScriptModuleInterface> ScriptModuleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class ScriptServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ScriptService")

	public:
		virtual void setWrapper( const ConstString& _type, ScriptWrapperInterface * _wrapper ) = 0;
		virtual ScriptWrapperInterface * getWrapper( const ConstString & _type ) const = 0;

	public:
		virtual bool bootstrapModules() = 0;
		virtual bool initializeModules() = 0;
		virtual bool finalizeModules() = 0;

	public:
		virtual void addModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) = 0;
		virtual void removeModulePath( const ConstString & _pack, const TVectorScriptModulePack & _modules ) = 0;

		virtual ScriptModuleInterfacePtr importModule( const ConstString& _name ) = 0;

		virtual void setCurrentModule( ScriptObject * _module ) = 0;
        virtual void addGlobalModule( const Char * _name, ScriptObject * _module ) = 0;
		virtual void removeGlobalModule( const Char * _name ) = 0;

    public:
        virtual bool stringize( ScriptObject * _object, ConstString & _str ) = 0;
	};

#   define SCRIPT_SERVICE( serviceProvider )\
    ((Menge::ScriptServiceInterface*)SERVICE_GET(serviceProvider, Menge::ScriptServiceInterface))
}
