#include "TaskReleasePrint.h"

#include "Kernel/Logger.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskReleasePrint::TaskReleasePrint( const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        int32_t message_size = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        m_message.assign( message, message_size );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskReleasePrint::TaskReleasePrint( const Char * _format, MENGINE_VA_LIST_TYPE _args )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        int32_t message_size = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, _args );

        m_message.assign( message, message_size );
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