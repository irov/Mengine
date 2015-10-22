#   pragma once

#   include "Interface/DataInterface.h"

#   include "Factory/FactoryDefault.h"

#	include "Logger/Logger.h"

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
			DataflowInterfacePtr decoder = m_factory.createObject();

			decoder->setServiceProvider( m_serviceProvider );

			if( decoder->initialize() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("DataflowFactory::createDataflow invalid initalize"
					);

				return nullptr;
			}

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