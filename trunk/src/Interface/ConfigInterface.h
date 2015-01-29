#	pragma once

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"
#	include "Core/Resolution.h"
#	include "Core/Viewport.h"
#	include "Core/AspectRatioViewports.h"
#	include "Core/ColourValue.h"
#	include "Core/Params.h"

namespace Menge
{
	class ConfigServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ConfigService")

	public:
		virtual bool initialize( const ConstString & _platformName ) = 0;
		virtual void finalize() = 0;

	public:
		virtual bool loadConfig( const ConstString & _fileGroup, const ConstString & _applicationPath ) = 0;

	public:
		virtual void setValue( const Char * _section, const Char * _key, const Char * _value ) = 0;

	public:
		virtual bool getValue( const Char * _section, const Char * _key, bool _default ) const = 0;
		virtual int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const = 0;
		virtual uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const = 0;
		virtual float getValue( const Char * _section, const Char * _key, float _default ) const = 0;
		virtual String getValue( const Char * _section, const Char * _key, const Char * _default ) const = 0;
		virtual WString getValue( const Char * _section, const Char * _key, const WChar * _default ) const = 0;
		virtual ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const = 0;
		virtual Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const = 0;		

	public:
		virtual void getValues( const Char * _section, const Char * _key, TVectorAspectRatioViewports & _value ) const = 0;
		virtual void getValues( const Char * _section, const Char * _key, TVectorWString & _value ) const = 0;
		virtual void getValues( const Char * _section, const Char * _key, TVectorString & _value ) const = 0;

	public:
		virtual void getSection( const Char * _section, TMapParams & _params ) const = 0;
	};

#   define CONFIG_SERVICE( serviceProvider )\
    ((Menge::ConfigServiceInterface *)SERVICE_GET(serviceProvider, Menge::ConfigServiceInterface))

#	define CONFIG_VALUE( serviceProvider, section, key, default )\
	(CONFIG_SERVICE( serviceProvider )->getValue( section, key, default ))

#	define CONFIG_SET( serviceProvider, section, key, value )\
	(CONFIG_SERVICE( serviceProvider )->setValue( section, key, value ))

#	define CONFIG_VALUES( serviceProvider, section, key, value )\
	(CONFIG_SERVICE( serviceProvider )->getValues( section, key, value ))

#	define CONFIG_SECTION( serviceProvider, section, params )\
	(CONFIG_SERVICE( serviceProvider )->getSection( section, params ))
}

