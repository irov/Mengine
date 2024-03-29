#pragma once

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "MemoryConfig.h"
#include "MultiConfig.h"

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
#   include "PersistentConfig.h"
#endif

#include "Kernel/ServiceBase.h"
#include "Kernel/Tags.h"

namespace Mengine
{
    class ConfigService
        : public ServiceBase<ConfigServiceInterface>
    {
    public:
        ConfigService();
        ~ConfigService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ConfigInterfacePtr createMemoryConfig( const DocumentInterfacePtr & _doc ) override;

    public:
        ConfigInterfacePtr loadConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc ) override;

    public:
        bool loadDefaultConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc ) override;

    public:
        const ConfigInterfacePtr & getDefaultConfig() const override;

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
    public:
        const ConfigInterfacePtr & getPersistentConfig() const override;
#endif

    protected:
        ThreadMutexInterfacePtr m_mutex;

        MultiConfigPtr m_defaultConfig;

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
        PersistentConfigPtr m_persistentConfig;
#endif

        FactoryInterfacePtr m_factoryMemoryConfig;
    };
}