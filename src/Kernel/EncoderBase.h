#pragma once

#include "Interface/EncoderInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Config/TypeTraits.h"

namespace Mengine
{
    template<class T>
    class EncoderBase
        : public T
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
        bool initialize( const ContentInterfacePtr & _content, const OutputStreamInterfacePtr & _stream ) override
        {
            if( m_initialize == true )
            {
                return false;
            }

            m_content = _content;
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

            if( m_content != nullptr )
            {
                m_content->closeOutputStreamFile( m_stream );
            }

            m_content = nullptr;
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
        const ContentInterfacePtr & getContent() const override
        {
            return m_content;
        }

        OutputStreamInterfacePtr getStream() const override
        {
            return m_stream;
        }

    protected:
        ContentInterfacePtr m_content;
        OutputStreamInterfacePtr m_stream;

        bool m_initialize;
    };
}
