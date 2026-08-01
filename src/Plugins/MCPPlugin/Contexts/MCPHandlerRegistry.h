#pragma once

#include "../MCPHandlerInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPHandlerRegistry
    {
    public:
        bool addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler );
        void removeHandler( const MCPHandlerInterfacePtr & _handler );
        MCPHandlerInterfacePtr findHandler( const ConstString & _name ) const;
        void clear();

    protected:
        typedef Map<ConstString, MCPHandlerInterfacePtr> MapHandlers;

        MapHandlers m_handlers;
    };
    //////////////////////////////////////////////////////////////////////////
}
