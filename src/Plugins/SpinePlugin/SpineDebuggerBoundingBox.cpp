#include "SpineDebuggerBoundingBox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SpineDebuggerBoundingBox::SpineDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpineDebuggerBoundingBox::~SpineDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpineDebuggerBoundingBox::_getBoundingBox( const SpinePtr & _node, mt::box2f * const _bb ) const
    {
        mt::box2f bb;
        if( _node->getWorldBoundingBox( &bb ) == false )
        {
            return false;
        }

        *_bb = bb;

        return true;
    };
}
