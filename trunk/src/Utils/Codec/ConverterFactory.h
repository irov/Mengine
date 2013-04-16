#   pragma once

#   include "Interface/ConverterInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    template<class T>
    class ConverterFactory
        : public ConverterFactoryInterface
    {
    public:
        ConverterFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
            : m_serviceProvider(_serviceProvider)
            , m_name(_name)
        {
        }

        virtual ~ConverterFactory()
        {
        }

    protected:
        ConverterInterface * createConverter() override
        {				
            return new T(m_serviceProvider);
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
    };
}