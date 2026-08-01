#include "MCPHandlerRegistry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPHandlerRegistry::addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler )
    {
        if( _name.empty() == true || _handler == nullptr )
        {
            return false;
        }

        bool successful = m_handlers.emplace( _name, _handler ).second;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPHandlerRegistry::removeHandler( const MCPHandlerInterfacePtr & _handler )
    {
        for( MapHandlers::iterator
            it = m_handlers.begin();
            it != m_handlers.end(); )
        {
            if( it->second == _handler )
            {
                it = m_handlers.erase( it );

                continue;
            }

            ++it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MCPHandlerInterfacePtr MCPHandlerRegistry::findHandler( const ConstString & _name ) const
    {
        MapHandlers::const_iterator it = m_handlers.find( _name );

        if( it == m_handlers.end() )
        {
            return nullptr;
        }

        MCPHandlerInterfacePtr handler = it->second;

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPHandlerRegistry::clear()
    {
        m_handlers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
