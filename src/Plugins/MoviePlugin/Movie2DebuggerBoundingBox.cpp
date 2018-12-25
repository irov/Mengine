#include "Movie2DebuggerBoundingBox.h"

namespace Mengine
{
    bool Movie2DebuggerBoundingBox::_getBoundingBox( const Movie2Ptr & _node, mt::box2f * _bb )
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
