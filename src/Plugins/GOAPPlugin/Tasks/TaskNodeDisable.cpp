#include "TaskNodeDisable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskNodeDisable::TaskNodeDisable( GOAP::Allocator * _allocator, const NodePtr & _node )
        : TaskInterface( _allocator )
        , m_node( _node )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskNodeDisable::~TaskNodeDisable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskNodeDisable::_onRun( GOAP::NodeInterface * _node )
    {
        MENGINE_UNUSED( _node );

        m_node->disable();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskNodeDisable::_onFinally()
    {
        m_node = nullptr;
    }
}