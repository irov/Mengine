#pragma once

#include "Interface/ServiceProviderInterface.h"
#include "Interface/PluginInterface.h"

#include "Kernel/FactorablePlugin.h"
#include "Kernel/MemoryAllocator.h"

#include "Config/Char.h"
#include "Config/Export.h"
#include "Config/UniqueId.h"

//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FUNCTION(Name)\
    MENGINE_PP_CONCATENATE(initMenginePlugin, Name)
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_STATIC(Name, Type)\
    extern "C"{bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid, bool _dynamic, bool _framework ){\
    if( _dynamic == true ){SERVICE_PROVIDER_SETUP(_serviceProvider);}\
    Mengine::PluginInterface * plugin = Mengine::Helper::newT<Mengine::FactorablePlugin<Type>>();\
    if( plugin == nullptr ){ return false; }\
    plugin->setUniqueIdentity( _uid );\
    plugin->setDynamicLoad( _dynamic );\
    plugin->setFrameworkMode( _framework );\
    *_plugin = plugin;\
    return true;}}
//////////////////////////////////////////////////////////////////////////
#ifndef PLUGIN_FACTORY_CREATE_FUNCTION
#define PLUGIN_FACTORY_CREATE_FUNCTION dllMengineCreatePlugin
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_CREATE_FUNCTION_NAME MENGINE_PP_STRINGIZE(PLUGIN_FACTORY_CREATE_FUNCTION)
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_DYNAMIC(Name, Type)\
    extern "C"\
    {\
        bool PLUGIN_FACTORY_CREATE_FUNCTION( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid )\
        {\
            return PLUGIN_FUNCTION(Name)( _serviceProvider, _plugin, _uid, true, false );\
        }\
    }
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_SHARED)
#   define PLUGIN_FACTORY(Name, Type)\
    PLUGIN_FACTORY_STATIC(Name, Type)\
    PLUGIN_FACTORY_DYNAMIC(Name, Type)\
    struct MENGINE_PP_CONCATENATE(__mengine_dummy_factory, Name){}
#else
#   define PLUGIN_FACTORY(Name, Type)\
    PLUGIN_FACTORY_STATIC(Name, Type)\
    struct MENGINE_PP_CONCATENATE(__mengine_dummy_factory, Name){}
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_EXPORT(Name)\
    extern "C"\
    {\
        extern bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid, bool _dynamic, bool _framework );\
    }
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_SERVICE_WAIT( Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(this->getPluginName().c_str(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_SERVICE_LEAVE( Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(this->getPluginName().c_str(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
