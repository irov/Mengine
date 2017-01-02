#   pragma once

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class ServiceProviderInterface;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ServantBase
        : public T
    {
    public:
        ServantBase()
            : m_serviceProvider( nullptr )
        {
        }

        virtual ~ServantBase()
        {
        }

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override
        {
            m_serviceProvider = _serviceProvider;
        }

        ServiceProviderInterface * getServiceProvider() const override
        {
            return m_serviceProvider;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;
    };
}