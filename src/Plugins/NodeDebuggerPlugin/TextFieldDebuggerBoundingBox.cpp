#include "TextFieldDebuggerBoundingBox.h"

namespace Mengine
{
    bool TextFieldDebuggerBoundingBox::_getBoundingBox( const TextFieldPtr & _node, mt::box2f * _bb )
    {
        Viewport vp;
        _node->calcTextViewport( vp );

        const mt::mat4f & wm = _node->getWorldMatrix();

        Viewport vpwm;
        vp.multiply( vpwm, wm );

        _bb->minimum = vpwm.begin;
        _bb->maximum = vpwm.end;

        return true;
    };
}
