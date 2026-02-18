#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ScriptEmbeddable.h"
#include "Kernel/ServiceRequiredList.h"
#include "Kernel/ConstString.h"

#include "Config/UniqueId.h"

#ifndef MENGINE_PLUGIN_NAME_MAX
#define MENGINE_PLUGIN_NAME_MAX 64
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class PluginInterface> PluginInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PluginInterface
        : public Factorable
        , public ScriptEmbeddable
    {
    public:
        virtual const ConstString & getPluginName() const = 0;

    public:
        virtual void setDynamicLoad( bool _dynamicLoad ) = 0;
        virtual bool isDynamicLoad() const = 0;

    public:
        virtual void setFrameworkMode( bool _frameworkMode ) = 0;
        virtual bool isFrameworkMode() const = 0;

    public:
        virtual const ServiceRequiredList & requiredServices() const = 0;

    public:
        virtual bool registerPlugin( const PluginInterfacePtr & _plugin ) = 0;
        virtual void unregisterPlugin( const PluginInterfacePtr & _plugin ) = 0;

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
        static_assert(sizeof(#Name) - 1 <= MENGINE_PLUGIN_NAME_MAX, "Plugin name too long");\
        const Mengine::ConstString & getPluginName() const override { return STRINGIZE_STRING_LOCAL_I(Name); };\
    protected:
//////////////////////////////////////////////////////////////////////////
