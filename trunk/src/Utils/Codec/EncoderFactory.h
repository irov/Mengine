#   pragma once

#   include "Interface/CodecInterface.h"

namespace Menge
{
    template<class T>
    class EncoderFactory
        : public EncoderFactoryInterface
    {
    public:
        EncoderFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
            : m_serviceProvider(_serviceProvider)
            , m_name(_name)
        {
        }

        virtual ~EncoderFactory()
        {
        }

    protected:
        EncoderInterface * createEncoder() override
        {
            T * encoder = m_factory.createObjectT();

            return encoder;
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

        typedef FactoryPool<T, 8> TFacrotyDecoder;
        TFacrotyDecoder m_factory;
    };
}