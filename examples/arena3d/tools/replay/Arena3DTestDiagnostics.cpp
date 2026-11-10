// Minimal diagnostics for networking tests without the engine service bootstrap.
// Assertions and unexpected exceptions fail the test immediately.
#include "Kernel/Assertion.h"
#include "Kernel/ExceptionHelper.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"

namespace Mengine
{
    namespace Helper
    {
#if defined( MENGINE_ASSERTION_DEBUG_ENABLE )
        //////////////////////////////////////////////////////////////////////////
        AssertionOperator::AssertionOperator( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _function )
            : m_category( _category )
            , m_level( _level )
            , m_test( _test )
            , m_file( _file )
            , m_line( _line )
            , m_function( _function )
        {
        }
        //////////////////////////////////////////////////////////////////////////
        AssertionOperator::~AssertionOperator()
        {
            StdIO::fprintf( stderr, "%s:%d assertion: %s\n", m_file, m_line, m_test );
            StdLib::exit( EXIT_FAILURE );
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()() const
        {
            return *this;
        }
        //////////////////////////////////////////////////////////////////////////
        const AssertionOperator & AssertionOperator::operator()( const Char * _format, ... ) const
        {
            MENGINE_UNUSED( _format );

            return *this;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::ExceptionHelper( const Char * _file, int32_t _line, const Char * _function )
        : m_file( _file )
        , m_line( _line )
        , m_function( _function )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::~ExceptionHelper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ExceptionHelper::operator()( const Char * _format, ... ) const
    {
        MENGINE_UNUSED( _format );

        StdIO::fprintf( stderr, "%s:%d unexpected exception\n", m_file, m_line );
        StdLib::exit( EXIT_FAILURE );
    }
    //////////////////////////////////////////////////////////////////////////
}
