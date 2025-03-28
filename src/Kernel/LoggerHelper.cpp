#include "LoggerHelper.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/StringRegex.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Char getLoggerLevelSymbol( ELoggerLevel _level )
        {
            switch( _level )
            {
            case LM_SILENT:
                return 'S';
            case LM_FATAL:
                return 'F';
            case LM_MESSAGE_RELEASE:
                return 'R';
            case LM_ERROR:
                return 'E';
            case LM_WARNING:
                return 'W';
            case LM_MESSAGE:
                return 'M';
            case LM_INFO:
                return 'I';
            case LM_DEBUG:
                return 'D';
            case LM_VERBOSE:
                return 'V';
            }

            return '?';
        }
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerFullDate( Timestamp _timestamp, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity )
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getDateTimeFromMilliseconds( _timestamp, &dateTime );

            int32_t size = MENGINE_SNPRINTF( _buffer + _offset, _capacity - _offset, _format //"[%02u:%02u:%02u:%04u]"
                , dateTime.year
                , dateTime.month
                , dateTime.day
                , dateTime.hour
                , dateTime.minute
                , dateTime.second
                , dateTime.milliseconds
            );

            return (size_t)size;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerShortDate( Timestamp _timestamp, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity )
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getDateTimeFromMilliseconds( _timestamp, &dateTime );

            int32_t size = MENGINE_SNPRINTF( _buffer + _offset, _capacity - _offset, _format //"[%02u:%02u:%02u:%04u]"
                , dateTime.hour
                , dateTime.minute
                , dateTime.second
                , dateTime.milliseconds
            );

            return (size_t)size;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerFunctionStamp( const Char * _function, int32_t _line, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity )
        {
            if( _function == nullptr )
            {
                return 0;
            }

            String str_function( _function );

            StringRegex regex_lambda_remove( "::<lambda_.*>::operator \\(\\)" );

            StringMatchResults match_lambda_remove;
            while( Regex::regex_search( str_function, match_lambda_remove, regex_lambda_remove ) == true )
            {
                const Regex::sub_match<String::const_iterator> & lambda_remove_prefix = match_lambda_remove.prefix();
                const Regex::sub_match<String::const_iterator> & lambda_remove_suffix = match_lambda_remove.suffix();

                str_function = String( lambda_remove_prefix.first, lambda_remove_prefix.second ) + String( lambda_remove_suffix.first, lambda_remove_suffix.second );
            }

            StringRegex regex_engine_remove( "Mengine::" );

            StringMatchResults match_engine_remove;
            if( Regex::regex_search( str_function, match_engine_remove, regex_engine_remove ) == true )
            {
                const Regex::sub_match<String::const_iterator> & engine_remove_prefix = match_engine_remove.prefix();
                const Regex::sub_match<String::const_iterator> & engine_remove_suffix = match_engine_remove.suffix();

                str_function = String( engine_remove_prefix.first, engine_remove_prefix.second ) + String( engine_remove_suffix.first, engine_remove_suffix.second );
            }

            int32_t size = MENGINE_SNPRINTF( _buffer + _offset, _capacity - _offset, _format //"%s[%d] "
                , str_function.c_str()
                , _line
            );

            return (size_t)size;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t makeLoggerThreadStamp( const ConstString & _threadName, const Char * _format, Char * const _buffer, size_t _offset, size_t _capacity )
        {
            int32_t size = MENGINE_SNPRINTF( _buffer + _offset, _capacity - _offset, _format //"|%s|"
                , _threadName.c_str()
            );

            return (size_t)size;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
