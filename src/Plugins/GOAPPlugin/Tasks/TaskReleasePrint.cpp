#include "TaskReleasePrint.h"

#include "Interface/LoggerInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskReleasePrint::TaskReleasePrint( GOAP::Allocator * _allocator, const Char * _format, ... )
        : GOAP::TaskInterface( _allocator )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
            , _format
        );

        m_message.assign( message, size_vsnprintf );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskReleasePrint::TaskReleasePrint( GOAP::Allocator * _allocator, const Char * _format, MENGINE_VA_LIST_TYPE _args )
        : GOAP::TaskInterface( _allocator )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, _format, _args );

        MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
            , _format
        );

        m_message.assign( message, size_vsnprintf );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskReleasePrint::~TaskReleasePrint()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskReleasePrint::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        LOGGER_MESSAGE_RELEASE( "%s"
            , m_message.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}