#pragma once

#include "Interface/StreamInterface.h"

namespace Mengine
{
    class OutputStreamWriter
    {
    public:
        OutputStreamWriter( const OutputStreamInterfacePtr & _stream )
            : m_stream( _stream )
        {
        }

    public:
        template<class T>
        void writePOD( const T & _t )
        {
            const void * buff = (const void *)(&_t);
            m_stream->write( buff, sizeof( T ) );
        }

        template<class T>
        void writePODs( const T * _t, uint32_t _count )
        {
            const void * buff = (const void *)(_t);
            m_stream->write( buff, sizeof( T ) * _count );
        }

        MENGINE_INLINE bool write( const void * _t, size_t _size )
        {
            bool successful = m_stream->write( _t, _size );

            return successful;
        }

    protected:
        OutputStreamInterfacePtr m_stream;
    };
}