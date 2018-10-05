#pragma once

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ServiceBase
        : public T
    {
    public:
        ServiceBase() noexcept
            : m_initializeService( false )
            , m_stopService( false )
        {
        }

        ~ServiceBase() override
        {
        }

    private:
        bool initializeService() override
        {
            if( m_initializeService == true )
            {
                return true;
            }

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

            m_initializeService = false;

            this->_finalizeService();
        }

    public:
        bool isInitializeService() const override
        {
            return m_initializeService;
        }

    protected:
        virtual bool _initializeService()
        {
            //Empty

            return true;
        }

        virtual void _finalizeService()
        {
            //Empty
        }


    protected:
        void stopService() override
        {
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
            this->_destroy();

            delete this;
        }

    protected:
        bool m_initializeService;
        bool m_stopService;
    };
}