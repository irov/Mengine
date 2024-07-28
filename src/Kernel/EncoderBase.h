#pragma once

#include "Interface/EncoderInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "Config/TypeTraits.h"

namespace Mengine
{
    template<class T>
    class EncoderBase
        : public T
        , public Factorable
    {
        static_assert(TypeTraits::is_base_of<EncoderInterface, T>, "T must derive from DecoderInterface");

    public:
        EncoderBase()
            : m_initialize( false )
        {
        }

        ~EncoderBase() override
        {
        }

    public:
        bool initialize( const OutputStreamInterfacePtr & _stream ) override
        {
            if( m_initialize == true )
            {
                return false;
            }

            m_stream = _stream;

            m_initialize = this->_initialize();

            return m_initialize;
        }

        void finalize() override
        {
            if( m_initialize == false )
            {
                return;
            }

            this->_finalize();

            m_stream = nullptr;
        }

    protected:
        virtual bool _initialize()
        {
            return true;
        }

        virtual void _finalize()
        {
            //Empty
        }

    protected:
        void _destroy() override
        {
            this->finalize();
        }

    public:
        OutputStreamInterfacePtr getStream() const override
        {
            return m_stream;
        }

    protected:
        OutputStreamInterfacePtr m_stream;

        bool m_initialize;
    };
}
