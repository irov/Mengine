#   pragma once

#   include "Interface/CodecInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Core/ServantBase.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{     
    template<class T>
    class DecoderFactory
        : public ServantBase<DecoderFactoryInterface>
    {
    protected:
        DecoderInterfacePtr createDecoder() override
        {	
            T * decoder = m_factory.createObject();

			decoder->setServiceProvider( m_serviceProvider );

            return decoder;
        }

	protected:
		void destroy() override
		{
			delete this;
		}

    protected:
        typedef FactoryPoolStore<T, 8> TFactoryDecoder;
        TFactoryDecoder m_factory;
    };

    namespace Helper
    {
        template<class T>
        inline DecoderFactoryInterfacePtr registerDecoder( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            DecoderFactoryInterfacePtr decoder = new DecoderFactory<T>();

            decoder->setServiceProvider( _serviceProvider );

            CODEC_SERVICE( _serviceProvider )
                ->registerDecoder( Helper::stringizeString(_serviceProvider, _type), decoder );

            return decoder;
        }

        inline void unregisterDecoder( ServiceProviderInterface * _serviceProvider, const Char * _type )
        {
            CODEC_SERVICE( _serviceProvider )
                ->unregisterDecoder( Helper::stringizeString( _serviceProvider, _type) );
        }
    }
}