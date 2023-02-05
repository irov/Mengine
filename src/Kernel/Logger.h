#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DateTime.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdArg.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_LOGGER_ANALYZE
#   ifdef MENGINE_DEBUG
#       define MENGINE_LOGGER_ANALYZE 1
#   else
#       define MENGINE_LOGGER_ANALYZE 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_LOGGER_ANALYZE == 1
#   define MENGINE_LOGGER_ANALYZE_ENABLE
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
        LoggerOperator( const ConstString & _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, uint32_t _line, ELoggerFlag _flag);
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

        ELoggerFlag m_flag;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( CATEGORY, LEVEL, FILTER, COLOR, FILE, LINE, FLAG )\
    if( Mengine::Detail::loggerValidMessage(CATEGORY, LEVEL, FILTER) == false) {} else Mengine::LoggerOperator(CATEGORY, (LEVEL), (FILTER), (COLOR), (FILE), (LINE), (FLAG))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_CATEGORY_VERBOSE_LEVEL( LEVEL, FILTER, COLOR, FILE, LINE, FLAG )\
    LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL_I(MENGINE_LIBRARY_NAME), (LEVEL), (FILTER), (COLOR), (FILE), (LINE), (FLAG) )
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_FATAL, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_CRITICAL( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_CRITICAL, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, Mengine::ELF_FLAG_FULL )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0 , Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_PROTECTED( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_PROTECTED, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_WARNING( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_WARNING( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_WARNING, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE, Mengine::ELF_FLAG_FULL )( __VA_ARGS__ )
#define LOGGER_MESSAGE( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_MESSAGE_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#define LOGGER_INFO( Category, ... ) LOGGER_VERBOSE_LEVEL( STRINGIZE_STRING_LOCAL(Category), Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_FULL )( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_LOGGER_ANALYZE_ENABLE)
#   define LOGGER_PERFORMANCE( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_BLUE, nullptr, 0, Mengine::ELF_FLAG_FULL )( __VA_ARGS__ )
#   define LOGGER_STATISTIC( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN, nullptr, 0, Mengine::ELF_FLAG_NONE )( __VA_ARGS__ )
#else
#   define LOGGER_PERFORMANCE( ... )
#   define LOGGER_STATISTIC( ... )
#endif
//////////////////////////////////////////////////////////////////////////
