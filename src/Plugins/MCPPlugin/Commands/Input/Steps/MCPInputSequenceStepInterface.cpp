#include "MCPInputSequenceStepInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputSequenceStepResponse::MCPInputSequenceStepResponse()
        : completed( false )
        , resumeGeneration( 0 )
        , resumeTimestamp( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPInputSequenceStepRegistry::addStep( const ConstString & _name, const MCPInputSequenceStepInterfacePtr & _step )
    {
        if( _name.empty() == true || _step == nullptr )
        {
            return false;
        }

        bool successful = m_steps.emplace( _name, _step ).second;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    MCPInputSequenceStepInterfacePtr MCPInputSequenceStepRegistry::findStep( const ConstString & _name ) const
    {
        MapSteps::const_iterator it = m_steps.find( _name );

        if( it == m_steps.end() )
        {
            return nullptr;
        }

        MCPInputSequenceStepInterfacePtr step = it->second;

        return step;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPInputSequenceStepRegistry::clear()
    {
        m_steps.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
