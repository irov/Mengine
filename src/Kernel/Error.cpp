#include "Error.h"
#include "Logger.h"

#include "Interface/PlatformInterface.h"

#include "Config/StdIO.h"

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
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        LoggerOperator( LM_CRITICAL, 0, m_file, m_line )
            .logMessageArgs( _format, args );

        MENGINE_VA_LIST_END( args );

        PLATFORM_SERVICE()
            ->abort();
    }
}
