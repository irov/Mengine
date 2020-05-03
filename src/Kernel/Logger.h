#pragma once

#include "Kernel/LoggerLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdArg.h"

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
        const LoggerOperator & operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const;

    public:
        void logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const;
        void logMessage( const Char * _msg, uint32_t _size ) const;

    protected:
        int32_t makeTimeStamp( Char * _buffer, int32_t _offset ) const;
        int32_t makeFunctionStamp( Char * _buffer, int32_t _offset ) const;

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
#define LOGGER_FATAL( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_FATAL, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( Format, __VA_ARGS__ )
#define LOGGER_CRITICAL( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_CRITICAL, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( Format, __VA_ARGS__ )
#define LOGGER_ERROR( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( Format, __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( Format, __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_LOGGER_DEBUG
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_PERFOMANCE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( Format, __VA_ARGS__ )
#define LOGGER_STATISTIC( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_STATISTIC, Mengine::LCOLOR_GREEN, nullptr, 0 )( Format, __VA_ARGS__ )
#define LOGGER_WARNING( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_WARNING, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( Format, __VA_ARGS__ )
#define LOGGER_MESSAGE( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( Format, __VA_ARGS__ )
#define LOGGER_MESSAGE_WN( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 ).setNewline(false)( Format, __VA_ARGS__ )
#define LOGGER_INFO( Format, ... ) LOGGER_VERBOSE_LEVEL( Mengine::LM_INFO, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0 )( Format, __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#define LOGGER_PERFORMANCE( Format, ... )
#define LOGGER_STATISTIC( Format, ... )
#define LOGGER_WARNING( Format, ... )
#define LOGGER_MESSAGE( Format, ... )
#define LOGGER_MESSAGE_WN( Format, ... )
#define LOGGER_INFO( Format, ... )
//////////////////////////////////////////////////////////////////////////
#endif