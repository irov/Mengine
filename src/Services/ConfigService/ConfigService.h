#pragma once

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "MemoryConfig.h"
#include "MultiConfig.h"
#include "PersistentConfig.h"

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
        ConfigInterfacePtr createMemoryConfig( const DocumentPtr & _doc ) override;

    public:
        ConfigInterfacePtr loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc ) override;

    public:
        bool loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc ) override;

    public:
        const ConfigInterfacePtr & getDefaultConfig() const override;
        const ConfigInterfacePtr & getPersistentConfig() const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        MultiConfigPtr m_defaultConfig;

#ifndef MENGINE_BUILD_PUBLISH
        PersistentConfigPtr m_persistentConfig;
#endif

        FactoryInterfacePtr m_factoryMemoryConfig;
    };
}