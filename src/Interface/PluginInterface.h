#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ServiceProviderInterface.h"

#include "Kernel/FactorablePlugin.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/ServiceRequiredList.h"

#include "Config/Char.h"
#include "Config/Export.h"
#include "Config/UniqueId.h"

#ifndef MENGINE_PLUGIN_NAME_MAX
#define MENGINE_PLUGIN_NAME_MAX 64
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PluginInterface
        : public ServantInterface
    {
    public:
        virtual const Char * getPluginName() const = 0;

    public:
        virtual void setUID( UniqueId _uid ) = 0;
        virtual UniqueId getUID() const = 0;

    public:
        virtual void setDynamicLoad( bool _dynamicLoad ) = 0;
        virtual bool isDynamicLoad() const = 0;

    public:
        virtual const ServiceRequiredList & requiredServices() const = 0;

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
        const Mengine::Char * getPluginName() const override { return Name; };\
    protected:
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
#ifndef PLUGIN_FACTORY_CREATE_FUNCTION_NAME
#define PLUGIN_FACTORY_CREATE_FUNCTION_NAME dllCreatePlugin
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef PLUGIN_FACTORY_DESTROY_FUNCTION_NAME
#define PLUGIN_FACTORY_DESTROY_FUNCTION_NAME dllDestroyPlugin
#endif
//////////////////////////////////////////////////////////////////////////
#define PLUGIN_FACTORY_DYNAMIC(Name, Type)\
    extern "C"\
    {\
        MENGINE_EXPORT_API bool PLUGIN_FACTORY_CREATE_FUNCTION_NAME( Mengine::ServiceProviderInterface * _serviceProvider, Mengine::PluginInterface ** const _plugin, Mengine::UniqueId _uid )\
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
