#include "Assertion.h"

#include "Kernel/Logger.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ASSERTION_MAX_MESSAGE
#define MENGINE_ASSERTION_MAX_MESSAGE 8192
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        AssertionOperator::AssertionOperator( uint32_t _level, const Char * _test, const Char * _file, uint32_t _line )
            : m_level( _level )
            , m_test( _test )
            , m_file( _file )
            , m_line( _line )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        AssertionOperator::~AssertionOperator()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()() const
        {
            Assertion( m_level, m_test, m_file, m_line, "" );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()( const Char * _format, ... ) const
        {
            va_list argList;
            va_start( argList, _format );

            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = { 0 };
            vsnprintf( str_info, MENGINE_ASSERTION_MAX_MESSAGE, _format, argList );

            va_end( argList );

            Assertion( m_level, m_test, m_file, m_line, str_info );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _format, ... )
        {
            va_list argList;
            va_start( argList, _format );

            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = { 0 };
            vsnprintf( str_info, MENGINE_ASSERTION_MAX_MESSAGE, _format, argList );

            va_end( argList );

            LOGGER_VERBOSE_LEVEL( Mengine::LM_ERROR, nullptr, 0 )("File %s [line:%d] Assertion [%s]: %s"
                , _file
                , _line
                , _test
                , str_info);

            if( HAS_OPTION( "assertion" ) == false && CONFIG_VALUE( "Engine", "AssertionDebugBreak", false ) == false && _level != ASSERTION_LEVEL_FATAL )
            {
                return;
            }

#ifdef MENGINE_PLATFORM_WINDOWS
            if( IsDebuggerPresent() == TRUE )
            {
                DebugBreak();
            }
#endif
        }
    }
}