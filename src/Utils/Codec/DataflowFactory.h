#   pragma once

#   include "Interface/DataInterface.h"

#   include "Factory/FactoryDefault.h"

namespace Menge
{     
	template<class T>
	class DataflowFactory
		: public DataflowFactoryInterface
	{
	public:
		DataflowFactory( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
		{
		}

		~DataflowFactory()
		{
		}

	protected:
		DataflowInterfacePtr createDataflow()
		{	
			T * decoder = m_factory.createObjectT();

			decoder->setServiceProvider( m_serviceProvider );

			return decoder;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef FactoryDefaultStore<T> TFactoryDecoder;
		TFactoryDecoder m_factory;
	};
}