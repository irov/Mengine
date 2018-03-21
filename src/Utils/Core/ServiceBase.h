#   pragma once

#include "Core/ServantBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ServiceBase
        : public ServantBase<T>
    {
    public:
        ServiceBase()
            : m_initialize( false )
        {
        }

        virtual ~ServiceBase()
        {
        }

    private:
        bool initialize() override
        {
            if( m_initialize == true )
            {
                return true;
            }

            bool successful = this->_initialize();

            m_initialize = successful;

            return m_initialize;
        }

        void finalize() override
        {
            if( m_initialize == false )
            {
                return;
            }

            m_initialize = false;

            this->_finalize();
        }

    public:
        bool isInitialize() const override
        {
            return m_initialize;
        }

    protected:
        virtual bool _initialize()
        {
            //Empty

            return true;
        }

        virtual void _finalize()
        {
            //Empty
        }


    protected:
        void stop() override
        {
            this->_stop();
        }

    protected:
        virtual void _stop()
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
        virtual void _destroy()
        {
            //Empty
        }

    protected:
        bool m_initialize;
    };
}