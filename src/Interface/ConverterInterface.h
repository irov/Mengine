#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Factory/FactorablePtr.h"

#   include "Core/ConstString.h"

namespace Menge
{	
	struct ConverterOptions
	{		
		ConstString pakName; 

		FilePath inputFileName;
        FilePath outputFileName;
	};
	
	class ConverterInterface
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:
		virtual bool initialize() = 0;

    public:
        virtual void setOptions( ConverterOptions * _options ) = 0;

	public:
		virtual const String & getConvertExt() const = 0;

    public:
		virtual bool validateVersion( const InputStreamInterfacePtr & _stream ) const = 0;

	public:
		virtual bool convert() = 0;
	};

	typedef stdex::intrusive_ptr<ConverterInterface> ConverterInterfacePtr;

	class ConverterFactoryInterface
	{
	public:
		virtual ConverterInterfacePtr createConverter() = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual void destroy() = 0;
	};

	class ConverterServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ConverterService")

	public:
		virtual void registerConverter( const ConstString& _type, ConverterFactoryInterface * _interface ) = 0;
		virtual void unregisterConverter( const ConstString& _type ) = 0;

		virtual ConverterInterfacePtr createConverter( const ConstString & _type ) = 0; 

    public:
        virtual bool convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out ) = 0;
	};

#   define CONVERTER_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ConverterServiceInterface)
}	// namespace Menge
