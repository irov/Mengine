#include "StreamLogger.h"

#include "Interface/LoggerServiceInterface.h"

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
        MENGINE_ASSERTION_MEMORY_PANIC( m_stream );

        LOGGER_SERVICE()
            ->writeHistory( LoggerInterfacePtr::from( this ) );

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
    void StreamLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );

        m_stream->write( _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::flush()
    {
        m_stream->flush();
    }
}

