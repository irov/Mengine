#include "MCPWaitConditionInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPWaitConditionResponse::MCPWaitConditionResponse()
        : satisfied( false )
    {
        jpp::object value = jpp::make_object();

        details = value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPWaitConditionRegistry::addCondition( const ConstString & _name, const MCPWaitConditionInterfacePtr & _condition )
    {
        if( _name.empty() == true || _condition == nullptr )
        {
            return false;
        }

        bool successful = m_conditions.emplace( _name, _condition ).second;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    MCPWaitConditionInterfacePtr MCPWaitConditionRegistry::findCondition( const ConstString & _name ) const
    {
        MapConditions::const_iterator it = m_conditions.find( _name );

        if( it == m_conditions.end() )
        {
            return nullptr;
        }

        MCPWaitConditionInterfacePtr condition = it->second;

        return condition;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPWaitConditionRegistry::clear()
    {
        m_conditions.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
