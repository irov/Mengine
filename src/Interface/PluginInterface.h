#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/FactorablePlugin.h"

#include "Config/Char.h"
#include "Config/Export.h"

#ifndef MENGINE_PLUGIN_NAME_MAX
#define MENGINE_PLUGIN_NAME_MAX 64
#endif

namespace Mengine
{
    class PluginInterface
        : public Mixin
    {
    public:
        virtual const Char * getPluginName() const = 0;

    public:
        virtual void setDynamicLoad( bool _dynamicLoad ) = 0;
        virtual bool isDynamicLoad() const = 0;

    public:
        virtual bool initializePlugin() = 0;
        virtual void finalizePlugin() = 0;

    public:
        virtual bool isInitializePlugin() const = 0;
        virtual bool isAvailablePlugin() const = 0;
        virtual bool isSystemPlugin() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PluginInterface> PluginInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_DECLARE( Name )\
    public:\
		MENGINE_INLINE const Char * getPluginName() const override { return Name; };\
    protected:
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FUNCTION(Name)\
	initPlugin##Name
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_STATIC(Name, Type)\
	extern "C"{bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** _plugin, bool _dynamic ){\
	if( _dynamic == true ){SERVICE_PROVIDER_SETUP(_serviceProvider);stdex_allocator_initialize();}\
	Mengine::PluginInterface * plugin = new Mengine::FactorablePlugin<Type>();\
	if( plugin == nullptr ){ return false; }\
	plugin->setDynamicLoad( _dynamic );\
	*_plugin = plugin;\
	return true;}}
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_DYNAMIC(Name, Type)\
	extern "C"\
	{\
		DLL_EXPORT bool dllCreatePlugin( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** _plugin )\
		{\
			return PLUGIN_FUNCTION(Name)( _serviceProvider, _plugin, true );\
		}\
	}
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_DLL
#   define PLUGIN_FACTORY(Name, Type)\
	PLUGIN_FACTORY_STATIC(Name, Type)\
	PLUGIN_FACTORY_DYNAMIC(Name, Type)
#else
#	define PLUGIN_FACTORY(Name, Type)\
	PLUGIN_FACTORY_STATIC(Name, Type)
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_EXPORT(Name)\
	extern "C"\
	{\
		extern bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** _plugin, bool _dynamic );\
	}
