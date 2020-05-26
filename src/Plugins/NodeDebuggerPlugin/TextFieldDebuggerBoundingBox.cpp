#include "TextFieldDebuggerBoundingBox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextFieldDebuggerBoundingBox::TextFieldDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextFieldDebuggerBoundingBox::~TextFieldDebuggerBoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextFieldDebuggerBoundingBox::_getBoundingBox( const TextFieldPtr & _node, mt::box2f * _bb )
    {
        Viewport vp;
        if( _node->calcTextViewport( &vp ) == false )
        {
            return false;
        }

        const mt::mat4f & wm = _node->getWorldMatrix();

        Viewport vpwm;
        vp.multiply( vpwm, wm );

        _bb->minimum = vpwm.begin;
        _bb->maximum = vpwm.end;

        return true;
    };
}
