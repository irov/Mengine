#pragma once

#include "Interface/LoggerServiceInterface.h"

#include <stdarg.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoggerOperator
    {
    public:
        LoggerOperator( EMessageLevel _level, uint32_t _flag, const Char * _file, uint32_t _line );
        ~LoggerOperator();

    public:
        const LoggerOperator & operator()( const Char * _format, ... ) const;

    public:
        void logMessageTimeStamp() const;
        void logMessageArgs( const Char * _format, va_list _args ) const;
        void logMessageFunctionStamp( const Char * _msg, uint32_t _size ) const;
        void logMessage( const Char * _msg, uint32_t _size ) const;

    protected:
        EMessageLevel m_level;
        uint32_t m_flag;

        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( LEVEL, FUNCTION, LINE )\
	if( LOGGER_SERVICE()->validMessage(LEVEL, 0) == false) {} else Mengine::LoggerOperator(LEVEL, 0, (FUNCTION), (LINE))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_FATAL, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_CRITICAL\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_CRITICAL, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_ERROR\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_PERFOMANCE, nullptr, 0 )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_STATISTIC\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_STATISTIC, nullptr, 0 )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_WARNING\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_WARNING, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_MESSAGE\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, nullptr, 0 )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_INFO\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_INFO, nullptr, 0 )
//////////////////////////////////////////////////////////////////////////