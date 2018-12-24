#include "ShapeDebuggerBoundingBox.h"

namespace Mengine
{
    bool ShapeDebuggerBoundingBox::_updateBoundingBox( const ShapePtr & _node, mt::box2f * _bb )
    {
        const mt::box2f * bb = _node->getBoundingBox();

        if( bb == nullptr )
        {
            return false;
        }

        *_bb = *bb;

        return true;
    };
}
