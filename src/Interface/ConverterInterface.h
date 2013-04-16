#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
	class ConverterServiceInterface;

	//struct ConverterDataInfo 
	//{
	//	FilePath outputFileName;
	//};
	
	struct ConverterOptions
	{		
		ConstString pakName; 

		FilePath inputFileName;
        FilePath outputFileName;
		//FilePath pakNameFullPath;
	};
	
	class ConverterInterface
	{
	public:
		virtual bool initialize() = 0;

    public:
        virtual void setOptions( ConverterOptions * _options ) = 0;

    public:
        virtual const String & getConvertExt() const = 0;

    public:
		virtual bool convert( bool & _skip ) = 0;

    public:
		virtual void destroy() = 0;
	};

	class ConverterFactoryInterface
	{
	public:
		virtual ConverterInterface * createConverter() = 0;
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

		template<class T>
		T * createConverterT( const ConstString& _type )
		{
			return dynamic_cast<T*>( createConverter( _type ) );
		}

		virtual ConverterInterface * createConverter( const ConstString & _type ) = 0; 

    public:
        virtual bool convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out ) = 0;
	};

#   define CONVERTER_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ConverterServiceInterface>(serviceProvider))
}	// namespace Menge
