#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DateTime.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdArg.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_LOGGER_DEBUG
#   ifdef MENGINE_DEBUG
#       define MENGINE_LOGGER_DEBUG 1
#   else
#       define MENGINE_LOGGER_DEBUG 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_LOGGER_DEBUG == 1
#   define MENGINE_LOGGER_DEBUG_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        bool loggerValidMessage( const ConstString & _category, ELoggerLevel _level, uint32_t _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    class LoggerOperator
    {
    public:
        LoggerOperator( const ConstString & _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, uint32_t _line );
        ~LoggerOperator();

    public:
        const LoggerOperator & operator()( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

    public:
        void logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const;

    protected:
        void logMessage( const PlatformDateTime & _dateTime, uint32_t _color, const Char * _data, size_t _size ) const;

    protected:
        ConstString m_category;

        ELoggerLevel m_level;
        uint32_t m_filter;
        uint32_t m_color;

        const Char * m_file;
        uint32_t m_line;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( CATEGORY, LEVEL, FLAG, COLOR, FILE, LINE )\
    if( Mengine::Detail::loggerValidMessage(CATEGORY, LEVEL, FLAG) == false) {} else Mengine::LoggerOperator(CATEGORY, (LEVEL), (FLAG), (COLOR), (FILE), (LINE))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_FATAL, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_CRITICAL( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_CRITICAL, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_ERROR( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_PROTECTED( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_PROTECTED, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_WN( NL, TS, ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 ).setNewline(NL).setTimestamp(TS)( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_ERROR( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0 )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_WARNING( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, nullptr, 0 )( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_LOGGER_DEBUG_ENABLE
#   define LOGGER_PERFORMANCE( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_PERFOMANCE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_STATISTIC( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_STATISTIC, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_WARNING( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_WARNING, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE )( __VA_ARGS__ )
#   define LOGGER_MESSAGE( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_MESSAGE_ERROR( ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0 )( __VA_ARGS__ )
#   define LOGGER_MESSAGE_WN( NL, TS, ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 ).setNewline(NL).setTimestamp(TS)( __VA_ARGS__ )
#   define LOGGER_INFO( Category, ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL(Category), Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0 )( __VA_ARGS__ )
#else
#   define LOGGER_PERFORMANCE( ... )
#   define LOGGER_STATISTIC( ... )
#   define LOGGER_WARNING( ... )
#   define LOGGER_MESSAGE( ... )
#   define LOGGER_MESSAGE_ERROR( ... )
#   define LOGGER_MESSAGE_WN( NL, TS, ... )
#   define LOGGER_INFO( Category, ... )
#endif
//////////////////////////////////////////////////////////////////////////