#include "StringArguments.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StringArguments::StringArguments()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringArguments::~StringArguments()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void StringArguments::addArgument( const Char * _argument )
    {
        m_arguments.push_back( String( _argument ) );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * StringArguments::getArgument( uint32_t _index ) const
    {
        const String & argument = m_arguments[_index];

        const Char * argument_str = argument.c_str();

        return argument_str;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t StringArguments::getArgumentCount() const
    {
        uint32_t count = (uint32_t)m_arguments.size();

        return count;
    }

}