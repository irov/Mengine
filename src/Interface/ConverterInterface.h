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

		String params;
	};
	
	class ConverterInterface
		: public ServantInterface
	{
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
        : public ServantInterface
	{
    public:
        virtual bool initialize() = 0;

	public:
		virtual ConverterInterfacePtr createConverter() = 0;
	};

    typedef stdex::intrusive_ptr<ConverterFactoryInterface> ConverterFactoryInterfacePtr;

	class ConverterServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("ConverterService")

	public:
		virtual bool registerConverter( const ConstString& _type, const ConverterFactoryInterfacePtr & _converter ) = 0;

    public:
		virtual bool unregisterConverter( const ConstString& _type ) = 0;

		virtual ConverterInterfacePtr createConverter( const ConstString & _type ) = 0; 

    public:
        virtual bool convert( const ConstString & _converter, const ConstString & _category, const FilePath & _in, FilePath & _out ) = 0;
	};

#   define CONVERTER_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ConverterServiceInterface)
}	// namespace Menge
