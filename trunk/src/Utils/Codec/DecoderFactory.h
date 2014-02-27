#   pragma once

#   include "Interface/CodecInterface.h"

#   include "Core/ConstString.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{     
    template<class T>
    class DecoderFactory
        : public DecoderFactoryInterface
    {
    public:
        DecoderFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
            : m_serviceProvider(_serviceProvider)
            , m_name(_name)
        {
        }

        ~DecoderFactory()
        {
        }

    protected:
        DecoderInterfacePtr createDecoder() override
        {	
            T * decoder = m_factory.createObjectT();

			decoder->setServiceProvider( m_serviceProvider );

            return decoder;
        }

        const ConstString & getName() const override
        {
            return m_name;
        }

	protected:
		void destroy() override
		{
			delete this;
		}

    protected:
        ServiceProviderInterface * m_serviceProvider;
        ConstString m_name;

        typedef FactoryPoolStore<T, 8> TFactoryDecoder;
        TFactoryDecoder m_factory;
    };
}