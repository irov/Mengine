#include "Assertion.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Crash.h"
#include "Kernel/Exception.h"
#include "Kernel/Logger.h"
#include "Kernel/Abort.h"
#include "Kernel/DebugBreak.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ASSERTION_MAX_MESSAGE
#define MENGINE_ASSERTION_MAX_MESSAGE 8192
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static bool Assertion_NotDebugBreak = true;
        //////////////////////////////////////////////////////////////////////////
        void AssertionSetNotDebugBreak( bool _debugBreak )
        {
            Assertion_NotDebugBreak = _debugBreak;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AssertionGetNotDebugBreak()
        {
            return Assertion_NotDebugBreak;
        }
        //////////////////////////////////////////////////////////////////////////
        AssertionOperator::AssertionOperator( EAssertionLevel _level, const Char * _test, const Char * _file, uint32_t _line )
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
            Helper::Assertion( m_level, m_test, m_file, m_line );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()( const Char * _format, ... ) const
        {
            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};

            MENGINE_VA_LIST_TYPE argList;
            MENGINE_VA_LIST_START( argList, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( str_info, MENGINE_ASSERTION_MAX_MESSAGE, _format, argList );
            MENGINE_UNUSED( size_vsnprintf );

            MENGINE_VA_LIST_END( argList );

            Helper::Assertion( m_level, m_test, m_file, m_line, "%s", str_info );

            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        void Assertion( EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line )
        {
            Helper::Assertion( _level, _test, _file, _line, "Unknown" );
        }
        //////////////////////////////////////////////////////////////////////////
        void Assertion( EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _format, ... )
        {
            Char str_info[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};

            MENGINE_VA_LIST_TYPE argList;
            MENGINE_VA_LIST_START( argList, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( str_info, MENGINE_ASSERTION_MAX_MESSAGE, _format, argList );
            MENGINE_UNUSED( size_vsnprintf );

            MENGINE_VA_LIST_END( argList );

            Char assertion_info[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};
            MENGINE_SNPRINTF( assertion_info, MENGINE_ASSERTION_MAX_MESSAGE, "File %s [line:%d] Assertion [%s]: %s"
                , _file
                , _line
                , _test
                , str_info
            );

            NOTIFICATION_NOTIFY( NOTIFICATOR_ASSERTION, _level, _test, _file, _line, str_info );
            
            if( _level == ASSERTION_LEVEL_CRITICAL )
            {
                Helper::crash( assertion_info );

                return;
            }

            if( _level == ASSERTION_LEVEL_EXCEPTION )
            {
                MENGINE_THROW_EXCEPTION( "%s"
                    , assertion_info
                );
                
                return;
            }

            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_ERROR, LFILTER_NONE, LCOLOR_RED, _file, _line, LFLAG_FULL )("%s"
                    , assertion_info
                    );
            }

            if( _level == ASSERTION_LEVEL_FATAL )
            {
                Helper::abort( assertion_info );
            }

            if( Assertion_NotDebugBreak == true && _level >= ASSERTION_LEVEL_WARNING )
            {
                return;
            }

            Helper::debuggerBreak();
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
