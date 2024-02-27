#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ConfigInterface.h"
#include "Interface/ContentInterface.h"

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
        virtual ConfigInterfacePtr loadConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool loadDefaultConfig( const ContentInterfacePtr & _content, const ConstString & _configType, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const ConfigInterfacePtr & getDefaultConfig() const = 0;

#if !defined(MENGINE_BUILD_PUBLISH_ENABLE)
    public:
        virtual const ConfigInterfacePtr & getPersistentConfig() const = 0;
#endif
    };
}
//////////////////////////////////////////////////////////////////////////
#define CONFIG_SERVICE()\
    ((Mengine::ConfigServiceInterface *)SERVICE_GET(Mengine::ConfigServiceInterface))
//////////////////////////////////////////////////////////////////////////

