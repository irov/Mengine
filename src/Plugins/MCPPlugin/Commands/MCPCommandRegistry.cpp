#include "MCPCommandRegistry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPCommandRegistry::addCommand( const ConstString & _name, const MCPCommandInterfacePtr & _command )
    {
        if( _name.empty() == true || _command == nullptr )
        {
            return false;
        }

        if( _command->setName_( _name ) == false )
        {
            return false;
        }

        bool successful = m_commands.emplace( _name, _command ).second;

        if( successful == false )
        {
            _command->m_name = ConstString::none();
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandRegistry::removeCommand( const MCPCommandInterfacePtr & _command )
    {
        for( MapCommands::iterator
            it = m_commands.begin();
            it != m_commands.end(); )
        {
            if( it->second == _command )
            {
                it->second->m_name = ConstString::none();
                it = m_commands.erase( it );

                continue;
            }

            ++it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MCPCommandInterfacePtr MCPCommandRegistry::findCommand( const ConstString & _name ) const
    {
        MapCommands::const_iterator it = m_commands.find( _name );

        if( it == m_commands.end() )
        {
            return nullptr;
        }

        MCPCommandInterfacePtr command = it->second;

        return command;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandRegistry::getCommandNames( VectorConstString * const _names ) const
    {
        for( const MapCommands::value_type & value : m_commands )
        {
            const ConstString & name = value.first;

            _names->emplace_back( name );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandRegistry::clear()
    {
        for( const MapCommands::value_type & value : m_commands )
        {
            value.second->m_name = ConstString::none();
        }

        m_commands.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
