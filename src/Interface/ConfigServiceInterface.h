#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class ConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConfigService" );

    public:
        virtual ConfigInterfacePtr createMemoryConfig( const Char * _doc ) = 0;

    public:
        virtual ConfigInterfacePtr loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) = 0;

    public:
        virtual bool loadDefaultConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) = 0;

    public:
        virtual const ConfigInterfacePtr & getDefaultConfig() const = 0;
    };

    template<class T>
    T decltype_string( T t );

    const Char * decltype_string( const Char * t );
}
//////////////////////////////////////////////////////////////////////////
#define CONFIG_SERVICE()\
    ((Mengine::ConfigServiceInterface *)SERVICE_GET(Mengine::ConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE( section, key, default )\
	([]() -> const decltype(Mengine::decltype_string(default)) &{ static decltype(Mengine::decltype_string( default )) value = CONFIG_SERVICE()->getDefaultConfig()->getValue( section, key, default );return value;}())
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUE2( section, key, default )\
	([default]() -> const decltype(Mengine::decltype_string(default)) &{ static decltype(Mengine::decltype_string(default)) value = CONFIG_SERVICE()->getDefaultConfig()->getValue( section, key, default ); return value;}())
////////////////////////////////////////////////////////////////////////////
#define CONFIG_VALUES(  section, key, value )\
	(CONFIG_SERVICE()->getDefaultConfig()->getValues( section, key, value ))
////////////////////////////////////////////////////////////////////////////

