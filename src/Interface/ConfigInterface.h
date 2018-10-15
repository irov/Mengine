#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Resolution.h"
#include "Kernel/AspectRatioViewports.h"
#include "Kernel/ColourValue.h"
#include "Kernel/Params.h"

namespace Mengine
{
    class ConfigServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConfigService" )

    public:
        virtual bool loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _applicationPath ) = 0;

    public:
        virtual bool getValue( const Char * _section, const Char * _key, bool _default ) const = 0;
        virtual int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const = 0;
        virtual uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const = 0;
        virtual uint64_t getValue( const Char * _section, const Char * _key, uint64_t _default ) const = 0;
        virtual float getValue( const Char * _section, const Char * _key, float _default ) const = 0;
        virtual double getValue( const Char * _section, const Char * _key, double _default ) const = 0;
        virtual String getValue( const Char * _section, const Char * _key, const Char * _default ) const = 0;
        virtual WString getValue( const Char * _section, const Char * _key, const WChar * _default ) const = 0;
        virtual ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const = 0;
        virtual FilePath getValue( const Char * _section, const Char * _key, const FilePath & _default ) const = 0;
        virtual Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const = 0;

    public:
        virtual void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _value ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorWString & _value ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorString & _value ) const = 0;

    public:
        virtual void getSection( const Char * _section, MapWParams & _params ) const = 0;
    };

#define CONFIG_SERVICE()\
    ((Mengine::ConfigServiceInterface *)SERVICE_GET(Mengine::ConfigServiceInterface))

#	define CONFIG_VALUE( section, key, default )\
	(CONFIG_SERVICE()->getValue( section, key, default ))

#	define CONFIG_SET( section, key, value )\
	(CONFIG_SERVICE()->setValue( section, key, value ))

#	define CONFIG_VALUES( section, key, value )\
	(CONFIG_SERVICE()->getValues( section, key, value ))

#	define CONFIG_SECTION( section, params )\
	(CONFIG_SERVICE()->getSection( section, params ))
}

