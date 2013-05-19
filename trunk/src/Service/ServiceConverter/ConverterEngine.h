#	pragma once

#	include "Interface/ConverterInterface.h"

#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class ConverterFactoryInterface;
	class ConverterInterface;


	class ConverterEngine
		: public ConverterServiceInterface
	{
	public:
		ConverterEngine();
		~ConverterEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		void registerConverter( const ConstString& _type, ConverterFactoryInterface * _interface ) override;
		void unregisterConverter( const ConstString& _type ) override;

		ConverterInterface * createConverter( const ConstString & _type ) override; 

    public:
        bool convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out ) override;
			
	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, ConverterFactoryInterface *> TMapConverterSystem;
		TMapConverterSystem m_mapConverterSystem;
	};
}
