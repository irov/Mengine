#include "TaskPrint.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskPrint::TaskPrint( const Char * _format, ... )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
            , _format
        );

        m_message.assign( message, size_vsnprintf );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPrint::TaskPrint( const Char * _format, MENGINE_VA_LIST_TYPE _args )
    {
        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, _args );

        MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid string format '%s'"
            , _format
        );

        m_message.assign( message, size_vsnprintf );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPrint::~TaskPrint()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPrint::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        LOGGER_MESSAGE( "%s"
            , m_message.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}