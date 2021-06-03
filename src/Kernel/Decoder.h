#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/Factorable.h"
#include "Kernel/Logger.h"

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

        ~Decoder() override
        {
        }

    public:
        bool initialize() override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::initialize" );

            if( m_initialize == true )
            {
                return false;
            }

            m_initialize = this->_initialize();

            return m_initialize;
        }

    protected:
        virtual bool _initialize()
        {
            //Empty

            return true;
        }

    public:
        void finalize() override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::finalize" );

            if( m_initialize == false )
            {
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
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::prepareData" );

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
            //Empty

            return true;
        }

    private:
        size_t decode( const DecoderData * _data ) override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::decode" );

            size_t byte = this->_decode( _data );

            return byte;
        }

    protected:
        virtual size_t _decode( const DecoderData * _data ) = 0;

    private:
        bool seek( float _time ) override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::seek" );

            bool successful = this->_seek( _time );

            return successful;
        }

    protected:
        virtual bool _seek( float _time )
        {
            MENGINE_UNUSED( _time );

            return false;
        }

    private:
        float tell() const override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::tell" );

            float value = this->_tell();

            return value;
        }

    protected:
        virtual float _tell() const
        {
            //Empty

            return 0.0;
        }

    private:
        bool rewind() override
        {
            MENGINE_THREAD_GUARD_SCOPE( Decoder, this, "Decoder::rewind" );

            bool successful = this->_rewind();

            return successful;
        }

    protected:
        virtual bool _rewind()
        {
            bool successful = m_stream->seek( m_rewindPos );

            return successful;
        }

    private:
        InputStreamInterfacePtr m_stream;

        size_t m_rewindPos;

        bool m_initialize;

        MENGINE_THREAD_GUARD_INIT( Decoder );
    };
}
