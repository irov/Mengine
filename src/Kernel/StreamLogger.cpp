#include "StreamLogger.h"

#include "Kernel/Document.h"
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
    bool StreamLogger::initialize()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_stream, false );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::finalize()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::log( ELoggerLevel _level, uint32_t _flag, const Char * _data, size_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );

        m_stream->write( _data, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::flush()
    {
        m_stream->flush();
    }
}

