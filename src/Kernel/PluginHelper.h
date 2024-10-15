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
    initMenginePlugin##Name
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_STATIC(Name, Type)\
    extern "C"{bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid, bool _dynamic ){\
    if( _dynamic == true ){SERVICE_PROVIDER_SETUP(_serviceProvider);}\
    Mengine::PluginInterface * plugin = Mengine::Helper::newT<Mengine::FactorablePlugin<Type>>();\
    if( plugin == nullptr ){ return false; }\
    plugin->setUID( _uid );\
    plugin->setDynamicLoad( _dynamic );\
    *_plugin = plugin;\
    return true;}}
//////////////////////////////////////////////////////////////////////////
#ifndef PLUGIN_FACTORY_CREATE_FUNCTION
#define PLUGIN_FACTORY_CREATE_FUNCTION dllCreatePlugin
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_CREATE_FUNCTION_NAME MENGINE_PP_STRINGIZE(PLUGIN_FACTORY_CREATE_FUNCTION)
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_DYNAMIC(Name, Type)\
    extern "C"\
    {\
        bool PLUGIN_FACTORY_CREATE_FUNCTION( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid )\
        {\
            return PLUGIN_FUNCTION(Name)( _serviceProvider, _plugin, _uid, true );\
        }\
    }
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_SHARED)
#   define PLUGIN_FACTORY(Name, Type)\
    PLUGIN_FACTORY_STATIC(Name, Type)\
    PLUGIN_FACTORY_DYNAMIC(Name, Type)
#else
#   define PLUGIN_FACTORY(Name, Type)\
    PLUGIN_FACTORY_STATIC(Name, Type)
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_EXPORT(Name)\
    extern "C"\
    {\
        extern bool PLUGIN_FUNCTION(Name)( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid, bool _dynamic );\
    }
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_SERVICE_WAIT( Type, Lambda )\
    SERVICE_PROVIDER_GET()->waitService(this->getPluginName(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_SERVICE_LEAVE( Type, Lambda )\
    SERVICE_PROVIDER_GET()->leaveService(this->getPluginName(), SERVICE_ID(Type), Lambda)
//////////////////////////////////////////////////////////////////////////
