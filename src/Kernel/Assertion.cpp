#include "Assertion.h"

#include "Kernel/Logger.h"

#include "Interface/OptionsInterface.h"
#include "Interface/ConfigServiceInterface.h"

#ifdef _WIN32
#   include "Environment/WIN32/WindowsIncluder.h"
#endif

#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ASSERTION_MAX_MESSAGE
#define MENGINE_ASSERTION_MAX_MESSAGE 8192
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AssertionOperator::AssertionOperator( const Char * _test, const Char * _file, uint32_t _line )
        : m_test( _test )
        , m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AssertionOperator::~AssertionOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AssertionOperator & AssertionOperator::operator()( const Char * _format, ... )
    {
        va_list argList;

        va_start( argList, _format );

        Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = { 0 };
        vsnprintf( str_info, MENGINE_ASSERTION_MAX_MESSAGE, _format, argList );

        va_end( argList );

        Assertion( m_test, m_file, m_line, str_info );

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    extern void Assertion( const Char * _test, const Char * _file, int _line, const Char * _info )
    {
        Char assert_message[2048];
        snprintf( assert_message, 2048, "File [%s:%d] Assertion: '%s' info '%s'"
            , _file
            , _line
            , _test
            , _info );

        LOGGER_ERROR( assert_message );

        if( HAS_OPTION( "assert" ) == false && CONFIG_VALUE( "Engine", "AssertionDebugBreak", false ) == false )
        {
            return;
        }

#ifdef _WIN32
        DebugBreak();
#endif
    }
}