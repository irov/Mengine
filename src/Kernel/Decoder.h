#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Logger.h"

#include "stdex/thread_guard.h"

namespace Mengine
{
    template<class TDecoderInterface>
    class Decoder
        : public TDecoderInterface
        , public Factorable
    {
    public:
        Decoder()
            : m_rewindPos( 0 )
            , m_initialize( false )
        {
        }

    public:
        bool initialize() override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::initialize" );

            if( m_initialize == true )
            {
                LOGGER_ERROR( "alredy initialize!"
                );

                return false;
            }

            m_initialize = this->_initialize();

            return m_initialize;
        }

    protected:
        virtual bool _initialize()
        {
            return true;
        }

    public:
        void finalize() override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::finalize" );

            if( m_initialize == false )
            {
                LOGGER_ERROR( "alredy finalize!"
                );

                return;
            }

            this->_finalize();

            m_stream = nullptr;
        }

    protected:
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
        const InputStreamInterfacePtr & getStream() const override
        {
            return m_stream;
        }

    private:
        bool prepareData( const InputStreamInterfacePtr & _stream ) override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::prepareData" );

            m_stream = _stream;

            if( this->_prepareData() == false )
            {
                return false;
            }

            m_rewindPos = m_stream->tell();

            return true;
        }

    protected:
        virtual bool _prepareData()
        {
            return true;
        }

    private:
        size_t decode( void * _buffer, size_t _bufferSize ) override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::decode" );

            size_t byte = this->_decode( _buffer, _bufferSize );

            return byte;
        }

    protected:
        virtual size_t _decode( void * _buffer, size_t _bufferSize ) = 0;

    private:
        bool seek( float _time ) override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::seek" );

            bool successful = this->_seek( _time );

            return successful;
        }

    protected:
        virtual bool _seek( float _timing )
        {
            MENGINE_UNUSED( _timing );

            return false;
        }

    private:
        float tell() const override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::tell" );

            float value = this->_tell();

            return value;
        }

    protected:
        virtual float _tell() const
        {
            return 0.0;
        }

    private:
        bool rewind() override
        {
            STDEX_THREAD_GUARD_SCOPE( this, "Decoder::rewind" );

            bool successful = this->_rewind();

            return successful;
        }

    protected:
        virtual bool _rewind()
        {
            bool successful = m_stream->seek( m_rewindPos );

            return successful;
        }

    protected:
        InputStreamInterfacePtr m_stream;

        size_t m_rewindPos;

        bool m_initialize;

        STDEX_THREAD_GUARD_INIT;
    };
}
