#pragma once

#include "Interface/ServiceProviderInterface.h"

#include "Kernel/Observable.h"
#include "Kernel/AssertionObservable.h"

namespace Mengine
{
    template<class T>
    class ServiceBase
        : public T
        , public Observable
    {
    public:
        ServiceBase() noexcept
            : m_availableService( false )
            , m_initializeService( false )
            , m_stopService( false )
        {
        }

        ~ServiceBase() override
        {
            MENGINE_ASSERTION_OBSERVABLE( this, "service '%s'"
                , SERVICE_ID(T)
            );
        }

    protected:
        const ServiceRequiredList & requiredServices() const override
        {
            static ServiceRequiredList required = {};

            return required;
        }

    private:
        bool availableService() override
        {
            m_availableService = this->_availableService();

            return m_availableService;
        }

        bool initializeService() override
        {
            if( m_availableService == false )
            {
                return true;
            }

            if( m_initializeService == true )
            {
                return true;
            }

            this->_dependencyService();

            bool successful = this->_initializeService();

            m_initializeService = successful;

            return m_initializeService;
        }

        void finalizeService() override
        {
            if( m_initializeService == false )
            {
                return;
            }

            const Char * serviceId = this->getServiceId();

            SERVICE_PROVIDER_GET()
                ->unlinkService( serviceId );

            m_initializeService = false;

            this->_finalizeService();
        }

        void replaceService() override
        {
            this->_replaceService();
        }

        bool runService() override
        {
            bool result = this->_runService();

            return result;
        }

    public:
        bool isAvailableService() const override
        {
            return m_availableService;
        }

        bool isInitializeService() const override
        {
            return m_initializeService;
        }

    protected:
        virtual void _dependencyService()
        {
            //Empty
        }

        virtual bool _availableService() const
        {
            return true;
        }

        virtual bool _initializeService()
        {
            //Empty

            return true;
        }

        virtual void _finalizeService()
        {
            //Empty
        }

        virtual void _replaceService()
        {
            //Empty
        }

        virtual bool _runService()
        {
            //Empty

            return true;
        }

    protected:
        void stopService() override
        {
            if( m_stopService == true )
            {
                return;
            }

            m_stopService = true;

            this->_stopService();
        }

        bool isStopService() const override
        {
            return m_stopService;
        }

    protected:
        virtual void _stopService()
        {
            //Empty       
        }

    private:
        void destroy() override
        {
            this->_destroyService();
            this->_destroy();

            delete this;
        }

        virtual void _destroyService()
        {
            //Empty
        }

    protected:
        bool m_availableService;
        bool m_initializeService;
        bool m_stopService;
    };
}