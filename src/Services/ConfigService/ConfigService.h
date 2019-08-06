#pragma once

#include "Interface/ConfigServiceInterface.h"

#include "IniConfig.h"
#include "MemoryConfig.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Tags.h"
#include "Kernel/Factory.h"

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
        ConfigInterfacePtr createMemoryConfig( const Char * _doc ) override;

    public:
        ConfigInterfacePtr loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) override;

    public:
        bool loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) override;

    public:
        const ConfigInterfacePtr & getDefaultConfig() const override;

    protected:
        Tags m_platformTags;

        INIConfigPtr m_defaultConfig;

        FactoryPtr m_factoryMemoryConfig;
        FactoryPtr m_factoryIniConfig;
    };
}