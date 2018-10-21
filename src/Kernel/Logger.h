#pragma once

#include "Interface/LoggerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LoggerOperator
    {
    public:
        LoggerOperator( EMessageLevel _level, uint32_t _flag, const Char * _function, uint32_t _line );
        ~LoggerOperator();

    public:
        LoggerOperator & operator()( const Char * _format, ... );

    public:
        void logMessage( const Char * _format, uint32_t _size );

    protected:
        EMessageLevel m_level;
        uint32_t m_flag;

        const Char * m_function;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( LEVEL, FUNCTION, LINE )\
	if( LOGGER_SERVICE()->validMessage(LEVEL, 0) == false) {} else Mengine::LoggerOperator(LEVEL, 0, (FUNCTION), (LINE))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_FATAL, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_CRITICAL\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_CRITICAL, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_ERROR\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_PERFOMANCE, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_STATISTIC\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_STATISTIC, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_WARNING\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_WARNING, __FUNCTION__, __LINE__ )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_INFO\
	LOGGER_VERBOSE_LEVEL( Mengine::LM_INFO, __FUNCTION__, __LINE__ )

