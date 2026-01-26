#pragma once

#include "Kernel/LoggerLevel.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DateTime.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdArg.h"

#if defined(MENGINE_DEBUG)
#   include "Config/StdIO.h"
#endif

//////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_LOGGER_INFO)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_LOGGER_INFO 1
#   else
#       define MENGINE_LOGGER_INFO 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_LOGGER_INFO == 1
#   define MENGINE_LOGGER_INFO_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_LOGGER_ANALYZE)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_LOGGER_ANALYZE 1
#   else
#       define MENGINE_LOGGER_ANALYZE 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        void loggerLockMessages();
        void loggerUnlockMessages();
        bool loggerValidMessage( const Char * _category, ELoggerLevel _level, uint32_t _filter );
    }
    //////////////////////////////////////////////////////////////////////////
    class LoggerLockScope
    {
    public:
        LoggerLockScope();
        ~LoggerLockScope();
    };
    //////////////////////////////////////////////////////////////////////////
    class LoggerOperator
    {
    public:
        LoggerOperator( const Char * _category, ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _file, int32_t _line, const Char * _function, uint32_t _flag );
        ~LoggerOperator();

    public:
        void operator()( const Char * _format, ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

    public:
        void logMessageArgs( const Char * _format, MENGINE_VA_LIST_TYPE _args ) const;

    public:
        void logMessage( uint32_t _color, const Char * _data, size_t _size ) const;

    protected:
        const Char * m_category;

        ELoggerLevel m_level;
        uint32_t m_filter;
        uint32_t m_color;

        const Char * m_file;
        const Char * m_function;
        int32_t m_line;

        uint32_t m_flag;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define LOGGER_LOCK_MESSAGES() Mengine::Detail::loggerLockMessages();
#define LOGGER_UNLOCK_MESSAGES() Mengine::Detail::loggerLockMessages();
#define LOGGER_SCOPE_MESSAGES_II(Index) Mengine::LoggerLockScope MENGINE_PP_CONCATENATE(logger_lock_scope_, Index)
#define LOGGER_SCOPE_MESSAGES_I(Index) LOGGER_SCOPE_MESSAGES_II(Index)
#define LOGGER_SCOPE_MESSAGES() LOGGER_SCOPE_MESSAGES_I(MENGINE_CODE_COUNTER)
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define LOGGER_VERBOSE_LEVEL( CATEGORY, LEVEL, FILTER, COLOR, FILE, LINE, FUNCTION, FLAG )\
    if( Mengine::Detail::loggerValidMessage(CATEGORY, LEVEL, FILTER) == false) {} else Mengine::LoggerOperator(CATEGORY, (LEVEL), (FILTER), (COLOR), (FILE), (LINE), (FUNCTION), (FLAG))
//////////////////////////////////////////////////////////////////////////
#define LOGGER_CATEGORY_VERBOSE_LEVEL( LEVEL, FILTER, COLOR, FLAG )\
    LOGGER_VERBOSE_LEVEL( MENGINE_CODE_LIBRARY, (LEVEL), (FILTER), (COLOR), MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, (FLAG) )
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
#   define LOGGER_CALL_MESSAGE_FORMAT( ... ) (__VA_ARGS__); static_assert(sizeof(MENGINE_PRINTF(__VA_ARGS__)))
#else
#   define LOGGER_CALL_MESSAGE_FORMAT( ... ) (__VA_ARGS__)
#endif
//////////////////////////////////////////////////////////////////////////
#define LOGGER_FATAL( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_FATAL, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, Mengine::LFLAG_FULL ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_ERROR, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, Mengine::LFLAG_FULL ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_ASSERTION( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, Mengine::LFLAG_FULL ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_PROTECTED( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_PROTECTED, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE_RELEASE_WARNING( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE_RELEASE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_WARNING( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_WARNING, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_GREEN, Mengine::LFLAG_FULL ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED | Mengine::LCOLOR_BLUE, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#define LOGGER_MESSAGE_ERROR( ... ) LOGGER_CATEGORY_VERBOSE_LEVEL( Mengine::LM_MESSAGE, Mengine::LFILTER_NONE, Mengine::LCOLOR_RED, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_LOGGER_INFO_ENABLE)
#   define LOGGER_INFO( Category, ... ) LOGGER_VERBOSE_LEVEL( Category, Mengine::LM_INFO, Mengine::LFILTER_NONE, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#   define LOGGER_INFO_PROTECTED( Category, ... ) LOGGER_VERBOSE_LEVEL( Category, Mengine::LM_INFO, Mengine::LFILTER_PROTECTED, Mengine::LCOLOR_GREEN | Mengine::LCOLOR_BLUE, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ );
#   define LOGGER_DEBUG( Category, ... ) LOGGER_VERBOSE_LEVEL( Category, Mengine::LM_DEBUG, Mengine::LFILTER_NONE, Mengine::LCOLOR_BLUE, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#   define LOGGER_VERBOSE( Category, ... ) LOGGER_VERBOSE_LEVEL( Category, Mengine::LM_VERBOSE, Mengine::LFILTER_NONE, Mengine::LCOLOR_NONE, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, Mengine::LFLAG_SHORT ) LOGGER_CALL_MESSAGE_FORMAT( __VA_ARGS__ )
#else
#   define LOGGER_INFO( Category, ... )
#   define LOGGER_INFO_PROTECTED( Category, ... )
#   define LOGGER_DEBUG( Category, ... )
#   define LOGGER_VERBOSE( Category, ... )
#endif
//////////////////////////////////////////////////////////////////////////
