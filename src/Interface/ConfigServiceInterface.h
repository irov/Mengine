#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Config/Typedef.h"
#include "Config/Metaprogramming.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConfigService" );

    public:
        virtual ConfigInterfacePtr createMemoryConfig( const DocumentPtr & _doc ) = 0;

    public:
        virtual ConfigInterfacePtr loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _configType, const DocumentPtr & _doc ) = 0;

    public:
        virtual const ConfigInterfacePtr & getDefaultConfig() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONFIG_SERVICE()\
    ((Mengine::ConfigServiceInterface *)SERVICE_GET(Mengine::ConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////

