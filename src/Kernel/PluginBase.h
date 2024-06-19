#pragma once

#include "Interface/PluginInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ModuleFactoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Observable.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"

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
        void setUID( UniqueId _uid ) override;
        UniqueId getUID() const override;

    protected:
        void setDynamicLoad( bool _dynamicLoad ) override;
        bool isDynamicLoad() const override;

    protected:
        const ServiceRequiredList & requiredServices() const override;

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
        bool addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc );

    protected:
        UniqueId m_uid;

        typedef Vector<ConstString> VectorModuleFactory;
        VectorModuleFactory m_moduleFactories;

        bool m_dynamicLoad;
        bool m_initializePlugin;
        bool m_availablePlugin;
        bool m_systemPlugin;
    };
}