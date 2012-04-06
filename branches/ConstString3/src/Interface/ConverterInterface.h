#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class ConverterServiceInterface;

	struct ConverterDataInfo 
	{
		WString outputFileName;
	};
	
	struct ConverterOptions
	{		
		ConstString pakName;
		WString inputFileName;
		WString pakNameFullPath;
	};
	
	class ConverterInterface
	{
	public:
		virtual void setOptions( ConverterOptions * _options ) = 0;

	public:
		virtual const ConverterDataInfo* getConverterDataInfo() const = 0;
	public:
		virtual bool initialize() = 0;
		virtual bool convert() = 0;
		virtual void destroy() = 0;
	};

	class ConverterSystemInterface
	{
	public:
		virtual void setService( ConverterServiceInterface * _service ) = 0;
		virtual ConverterInterface * createConverter() = 0;
	};


	class ConverterServiceInterface
		: public ServiceInterface
	{
	public:
		virtual void registerConverter( const ConstString& _type, ConverterSystemInterface * _interface ) = 0;
		virtual void unregisterConverter( const ConstString& _type ) = 0;
	
		template<class T>
		T * createConverterT( const ConstString& _type )
		{
			return dynamic_cast<T*>( createConverter( _type ) );
		}

		virtual ConverterInterface * createConverter( const ConstString & _type ) = 0; 
	};

}	// namespace Menge
