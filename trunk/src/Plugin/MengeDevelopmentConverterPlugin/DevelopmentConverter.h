#	pragma once

#	include "Interface/ConverterInterface.h"

namespace Menge
{
	class DevelopmentConverter
        : public ConverterInterface
	{
	public:
		DevelopmentConverter();
		~DevelopmentConverter();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		const String & getConvertExt() const override;

	public:
		void setOptions( ConverterOptions * _options ) override;

	public:
		bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ConverterOptions m_options;

		String m_convertExt;
	};
}
