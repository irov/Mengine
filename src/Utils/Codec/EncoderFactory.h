#   pragma once

#   include "Interface/CodecInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/ServantBase.h"

#	include "Factory/FactoryPool.h"

namespace Menge
{
    template<class T>
    class EncoderFactory
        : public ServantBase<EncoderFactoryInterface>
    {
    protected:
        bool initialize() override
        {
            m_factory = new FactoryPool<T, 8>( m_serviceProvider );

            return true;
        }

    protected:
        EncoderInterfacePtr createEncoder() override
        {
            T * encoder = m_factory->createObject();

			encoder->setServiceProvider( m_serviceProvider );

            return encoder;
        }

	protected:
		void destroy() override
		{
			delete this;
		}

    protected:
        FactoryPtr m_factory;
    };

    namespace Helper
    {
        template<class T>
        inline EncoderFactoryInterfacePtr registerEncoder( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            EncoderFactoryInterfacePtr encoder = new EncoderFactory<T>();

            encoder->setServiceProvider( _serviceProvider );
			
			if( encoder->initialize() == false )
			{
				return nullptr;
			}

            CODEC_SERVICE( _serviceProvider )
                ->registerEncoder( Helper::stringizeString( _serviceProvider, _type ), encoder );

            return encoder;
        }

        inline void unregisterEncoder( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            CODEC_SERVICE( _serviceProvider )
                ->unregisterEncoder( Helper::stringizeString( _serviceProvider, _type ) );
        }
    }
}