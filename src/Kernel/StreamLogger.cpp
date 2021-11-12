#include "StreamLogger.h"

#include "Kernel/Assertion.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::StreamLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::~StreamLogger()
    {
        MENGINE_ASSERTION_FATAL( m_stream == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::setStream( const OutputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    const OutputStreamInterfacePtr & StreamLogger::getStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StreamLogger::_initializeLogger()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::_finalizeLogger()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );

        m_stream->write( _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::flush()
    {
        m_stream->flush();
    }
    //////////////////////////////////////////////////////////////////////////
}

