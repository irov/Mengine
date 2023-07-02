#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConfigService" );

    public:
        virtual ConfigInterfacePtr createMemoryConfig( const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual ConfigInterfacePtr loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const ConfigInterfacePtr & getDefaultConfig() const = 0;

#ifndef MENGINE_BUILD_PUBLISH
    public:
        virtual const ConfigInterfacePtr & getPersistentConfig() const = 0;
#endif
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONFIG_SERVICE()\
    ((Mengine::ConfigServiceInterface *)SERVICE_GET(Mengine::ConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////

