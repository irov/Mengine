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
#define CONFIG_VALUE( section, key, default )\
    []() -> const decltype(Mengine::mpl::decltype_string(default)) &{ static decltype(Mengine::mpl::decltype_string( default )) value = SERVICE_IS_INITIALIZE(ConfigServiceInterface) == true ? CONFIG_SERVICE()->getDefaultConfig()->getValue( section, key, default ) : default; return value;}()
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE2( section, key, default )\
    [default]() -> const decltype(Mengine::mpl::decltype_string(default)) &{ static decltype(Mengine::mpl::decltype_string(default)) value = SERVICE_IS_INITIALIZE(ConfigServiceInterface) == true ? CONFIG_SERVICE()->getDefaultConfig()->getValue( section, key, default ) : default; return value;}()
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUET( section, key, default, type )\
    []() -> const type &{static type value = SERVICE_IS_INITIALIZE(ConfigServiceInterface) == true ? CONFIG_SERVICE()->getDefaultConfig()->getValue( section, key, (type)default ) : (type)default;return value;}()
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUES( section, key, value )\
    CONFIG_SERVICE()->getDefaultConfig()->getValues( section, key, value )
////////////////////////////////////////////////////////////////////////////

