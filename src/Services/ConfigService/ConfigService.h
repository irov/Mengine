#pragma once

#include "Interface/ConfigServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "MemoryConfig.h"
#include "MultiConfig.h"

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

    protected:
        ThreadMutexInterfacePtr m_mutex;

        MultiConfigPtr m_defaultConfig;

        FactoryInterfacePtr m_factoryMemoryConfig;
    };
}