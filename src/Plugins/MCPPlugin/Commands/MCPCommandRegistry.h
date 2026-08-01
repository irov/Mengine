#pragma once

#include "MCPCommandInterface.h"

#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPCommandRegistry
    {
    public:
        bool addCommand( const ConstString & _name, const MCPCommandInterfacePtr & _command );
        void removeCommand( const MCPCommandInterfacePtr & _command );
        MCPCommandInterfacePtr findCommand( const ConstString & _name ) const;

        void update();
        void cancel( uint32_t _requestId );
        void disconnect();
        void clear();

    protected:
        typedef Map<ConstString, MCPCommandInterfacePtr> MapCommands;

        MapCommands m_commands;
    };
    //////////////////////////////////////////////////////////////////////////
}
