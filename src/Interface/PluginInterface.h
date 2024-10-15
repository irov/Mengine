#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ServiceRequiredList.h"

#include "Config/Char.h"
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
