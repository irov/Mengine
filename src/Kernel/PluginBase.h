#pragma once

#include "Interface/PluginInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/Vector.h"
#include "Config/String.h"

namespace Mengine
{
    class PluginBase
        : public PluginInterface
        , public Factorable
    {
    public:
        PluginBase();
        ~PluginBase() override;

    protected:
        void setDynamicLoad( bool _dynamicLoad ) override;
        bool isDynamicLoad() const override;

    protected:
        bool initializePlugin() override;
        void finalizePlugin() override;

    protected:
        bool isInitializePlugin() const override;
        bool isAvailablePlugin() const override;
        bool isSystemPlugin() const override;

    protected:
        void destroy() override;

    protected:
        virtual bool _initializePlugin();
        virtual void _finalizePlugin();
        virtual void _destroyPlugin();
        virtual bool _availablePlugin() const;
        virtual bool _systemPlugin() const;

    protected:
        bool addModuleFactory( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory );

    protected:
        typedef Vector<ConstString> VectorModuleFactory;
        VectorModuleFactory m_moduleFactories;

        ThreadMutexInterfacePtr m_mutexAllocatorPool;

        bool m_dynamicLoad;
        bool m_initializePlugin;
        bool m_availablePlugin;
        bool m_systemPlugin;
    };
}