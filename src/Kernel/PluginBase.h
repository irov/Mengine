#pragma once

#include "Interface/PluginInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Observable.h"
#include "Kernel/BaseScriptEmbedded.h"

namespace Mengine
{
    class PluginBase
        : public PluginInterface
        , public Observable
    {
    public:
        PluginBase();
        ~PluginBase() override;

    protected:
        void setDynamicLoad( bool _dynamicLoad ) override;
        bool isDynamicLoad() const override;

    protected:
        void setFrameworkMode( bool _frameworkMode ) override;
        bool isFrameworkMode() const override;

    protected:
        const ServiceRequiredList & requiredServices() const override;

    protected:
        bool registerPlugin( const PluginInterfacePtr & _plugin ) override;
        void unregisterPlugin( const PluginInterfacePtr & _plugin ) override;

    protected:
        bool initializePlugin() override;
        void finalizePlugin() override;

    protected:
        bool isInitializePlugin() const override;
        bool isAvailablePlugin() const override;
        bool isSystemPlugin() const override;

    protected:
        void _destroy() override;

    protected:
        virtual bool _initializePlugin();
        virtual void _finalizePlugin();
        virtual void _destroyPlugin();
        virtual bool _availablePlugin() const;
        virtual bool _systemPlugin() const;
        virtual bool _unimportantPlugin() const;

    protected:
        bool addModuleFactory( const ConstString & _name, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc );
        void removeModuleFactory( const ConstString & _name );

    protected:
        bool m_dynamicLoad;
        bool m_frameworkMode;
        bool m_initializePlugin;
        bool m_availablePlugin;
        bool m_systemPlugin;
    };
}