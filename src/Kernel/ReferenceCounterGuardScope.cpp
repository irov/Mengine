#include "ReferenceCounterGuardScope.h"

#include "Interface/LoggerInterface.h"

#include "Kernel/Error.h"

#include "Config/StdArg.h"
#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounterGuardScope::ReferenceCounterGuardScope( Factorable * _factorable, const Char * _format, ... )
        : m_factorable( FactorablePtr::from( _factorable ) )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        MENGINE_VSNPRINTF( m_message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );
    }
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounterGuardScope::~ReferenceCounterGuardScope()
    {
        uint32_t refcount = m_factorable->getrefcount();

        if( refcount != 1 )
        {
            return;
        }

        MENGINE_ERROR_FATAL( "ReferenceCounterGuardScope: %s"
            , m_message
        );
    }
    //////////////////////////////////////////////////////////////////////////
}