#   pragma once

#   include "Interface/CodecInterface.h"

#   include "Core/ConstString.h"

#   include "Factory/FactoryPool.h"

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

        virtual ~DecoderFactory()
        {
        }

    protected:
        DecoderInterface * createDecoder() override
        {	
            T * decoder = m_factory.createObjectT();

            return decoder;
        }

        const ConstString & getName() const override
        {
            return m_name;
        }

        void destroy() override
        {
            delete this;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;
        ConstString m_name;

        typedef FactoryPool<T, 8> TFacrotyDecoder;
        TFacrotyDecoder m_factory;
    };
}