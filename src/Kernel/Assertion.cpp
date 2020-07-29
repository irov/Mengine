#include "Assertion.h"

#include "Kernel/Exception.h"
#include "Kernel/Logger.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/NotificationServiceInterface.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif

#include "Config/StdIO.h"

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
            Helper::Assertion( m_level, m_test, m_file, m_line, "" );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()( const Char * _format, ... ) const
        {
            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};

            MENGINE_VA_LIST_TYPE argList;
            MENGINE_VA_LIST_START( argList, _format );

            MENGINE_VSNPRINTF( str_info, MENGINE_ASSERTION_MAX_MESSAGE - 1, _format, argList );

            MENGINE_VA_LIST_END( argList );

            Helper::Assertion( m_level, m_test, m_file, m_line, "%s", str_info );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line )
        {
            Helper::Assertion( _level, _test, _file, _line, "" );
        }
        //////////////////////////////////////////////////////////////////////////
        void Assertion( uint32_t _level, const Char * _test, const Char * _file, int32_t _line, const Char * _format, ... )
        {
            if( _level == ASSERTION_LEVEL_CRITICAL )
            {
                volatile uint32_t * p = nullptr;
                *p = 0xBADC0DE;

                return;
            }

            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};

            MENGINE_VA_LIST_TYPE argList;
            MENGINE_VA_LIST_START( argList, _format );

            MENGINE_VSNPRINTF( str_info, MENGINE_ASSERTION_MAX_MESSAGE - 1, _format, argList );

            MENGINE_VA_LIST_END( argList );

            if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_ASSERTION, _level, _test, _file, _line, str_info );
            }

            if( _level == ASSERTION_LEVEL_EXCEPTION )
            {
                MENGINE_THROW_EXCEPTION( "%s"
                    , str_info
                );
            }

            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_VERBOSE_LEVEL( LM_ERROR, LCOLOR_RED, nullptr, 0 )("File %s [line:%d] Assertion [%s]: %s"
                    , _file
                    , _line
                    , _test
                    , str_info);
            }

            if( _level == ASSERTION_LEVEL_FATAL )
            {
                if( SERVICE_IS_INITIALIZE( PlatformInterface ) == true )
                {
                    PLATFORM_SERVICE()
                        ->abort();
                }
            }

            if( HAS_OPTION( "assertion" ) == false && CONFIG_VALUE( "Engine", "AssertionDebugBreak", false ) == false && _level >= ASSERTION_LEVEL_WARNING )
            {
                return;
            }

#ifdef MENGINE_PLATFORM_WINDOWS
            if( ::IsDebuggerPresent() == TRUE )
            {
                ::DebugBreak();
            }
#endif
        }
    }
}