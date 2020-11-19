#include "TaskNodeDestroy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskNodeDestroy::TaskNodeDestroy( const NodePtr & _node )
        : m_node( _node )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskNodeDestroy::~TaskNodeDestroy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskNodeDestroy::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        m_node->dispose();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskNodeDestroy::_onFinally()
    {
        m_node = nullptr;
    }
}