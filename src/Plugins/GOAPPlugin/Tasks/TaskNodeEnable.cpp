#include "TaskNodeEnable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskNodeEnable::TaskNodeEnable( const NodePtr & _node )
        : m_node( _node )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskNodeEnable::~TaskNodeEnable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskNodeEnable::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        m_node->enable();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskNodeEnable::_onFinally()
    {
        m_node = nullptr;
    }
}