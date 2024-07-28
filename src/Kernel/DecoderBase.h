#pragma once

#include "Interface/DecoderInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/Factorable.h"
#include "Kernel/Logger.h"

#include "Config/TypeTraits.h"

namespace Mengine
{
    template<class T>
    class DecoderBase
        : public T
        , public Factorable
    {
        static_assert(TypeTraits::is_base_of<DecoderInterface, T>, "T must derive from DecoderInterface");

    public:
        DecoderBase()
            : m_rewindPos( 0 )
            , m_initialize( false )
        {
        }

        ~DecoderBase() override
        {
        }

    public:
        bool initialize() override
        {
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

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
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

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
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

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
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

            size_t byte = this->_decode( _data );

            return byte;
        }

    protected:
        virtual size_t _decode( const DecoderData * _data ) = 0;

    private:
        bool seek( float _time ) override
        {
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

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
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

            float value = this->_tell();

            return value;
        }

    protected:
        virtual float _tell() const
        {
            //Empty

            return 0.f;
        }

    private:
        bool rewind() override
        {
            MENGINE_THREAD_GUARD_SCOPE( DecoderBase, this );

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

        MENGINE_THREAD_GUARD_INIT( DecoderBase );
    };
}
