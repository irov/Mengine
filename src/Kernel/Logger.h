#pragma once

#include "Kernel/LoggerLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

#include <stdarg.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        bool loggerValidMessage( ELoggerLevel _level, uint32_t _flag );
    }
    //////////////////////////////////////////////////////////////////////////
    class LoggerOperator
    {
    public:
        LoggerOperator( ELoggerLevel _level, uint32_t _flag, const Char * _file, uint32_t _line );
        ~LoggerOperator();

    public:
        LoggerOperator & setNewline( bool _newline );
        bool getNewline() const;

    public:
        const LoggerOperator & operator()( const Char * _format, ... ) const;

    public:
        void logMessageArgs( const Char * _format, va_list _args ) const;
        void logMessage( const Char * _msg, uint32_t _size ) const;

    protected:
        int32_t makeTimeStamp( Char * _buffer, int32_t _offset ) const;
        int32_t makeFunctionStamp( Char * _buffer, int32_t _offset ) const;

    protected:
        ELoggerLevel m_level;
        uint32_t m_flag;

        const Char * m_file;
        uint32_t m_line;

        bool m_newline;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( LEVEL, FILE, LINE )\
	if( Mengine::Detail::loggerValidMessage(LEVEL, 0) == false) {} else Mengine::LoggerOperator(LEVEL, 0, (FILE), (LINE))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL LOGGER_VERBOSE_LEVEL( Mengine::LM_FATAL, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
#define LOGGER_CRITICAL LOGGER_VERBOSE_LEVEL( Mengine::LM_CRITICAL, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
#define LOGGER_ERROR LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_LOGGER_DEBUG
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_PERFOMANCE, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_STATISTIC( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_STATISTIC, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_WARNING( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_WARNING, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_MESSAGE( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_MESSAGE_WN( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, nullptr, 0 ).setNewline(false)( __VA_ARGS__ )
#define LOGGER_INFO( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_INFO, nullptr, 0 )( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE( ... )
#define LOGGER_STATISTIC( ... )
#define LOGGER_WARNING( ... )
#define LOGGER_MESSAGE( ... )
#define LOGGER_MESSAGE_WN( ... )
#define LOGGER_INFO( ... )
//////////////////////////////////////////////////////////////////////////
#endif