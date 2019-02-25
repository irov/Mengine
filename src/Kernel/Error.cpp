#include "Error.h"
#include "Logger.h"

#include "Interface/PlatformInterface.h"

#include <stdarg.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ErrorOperator::ErrorOperator( const Char * _file, uint32_t _line )
        : m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ErrorOperator::~ErrorOperator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ErrorOperator::operator () ( const Char * _format, ... ) const
    {
        va_list args;
        va_start( args, _format );

        LoggerOperator( LM_CRITICAL, 0, m_file, m_line )
            .logMessageArgs( _format, args );

        va_end( args );

        PLATFORM_SERVICE()
            ->abort();
    }    
}
