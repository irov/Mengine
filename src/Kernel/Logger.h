#pragma once

#include "Kernel/LoggerLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdArg.h"

#ifndef MENGINE_LOGGER_MAX_MESSAGE
#define MENGINE_LOGGER_MAX_MESSAGE 16384
#endif

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
        LoggerOperator( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _file, uint32_t _line );
        ~LoggerOperator();

    public:
        LoggerOperator & setNewline( bool _newline );
        bool getNewline() const;

    public:
        const LoggerOperator & operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

    public:
        void logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const;
        void logMessage( const Char * _msg, size_t _size ) const;

    protected:
        ELoggerLevel m_level;
        uint32_t m_flag;
        uint32_t m_color;

        const Char * m_file;
        uint32_t m_line;

        bool m_newline;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( LEVEL, COLOR, FILE, LINE )\
    if( Mengine::Detail::loggerValidMessage(LEVEL, 0) == false) {} else Mengine::LoggerOperator((LEVEL), 0, (COLOR), (FILE), (LINE))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_FATAL, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_CRITICAL( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_CRITICAL, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_ERROR( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_LOGGER_DEBUG
#   define LOGGER_PERFORMANCE( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_PERFOMANCE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_STATISTIC( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_STATISTIC, Mengine::LCOLOR_GREEN, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_WARNING( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_WARNING, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#   define LOGGER_MESSAGE( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_MESSAGE_ERROR( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_MESSAGE_WN( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 ).setNewline(false)( __VA_ARGS__ )
#   define LOGGER_INFO( ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_INFO, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#else
#   define LOGGER_PERFORMANCE( ... )
#   define LOGGER_STATISTIC( ... )
#   define LOGGER_WARNING( ... )
#   define LOGGER_MESSAGE( ... )
#   define LOGGER_MESSAGE_ERROR( ... )
#   define LOGGER_MESSAGE_WN( ... )
#   define LOGGER_INFO( ... )
#endif
//////////////////////////////////////////////////////////////////////////