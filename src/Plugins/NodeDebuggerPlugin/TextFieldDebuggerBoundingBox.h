#pragma once

#include "NodeDebuggerBoundingBoxInterface.h"

#include "Engine/TextField.h"

namespace Mengine
{
    class TextFieldDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<TextFieldPtr>
        , public Factorable
    {
    public:
        bool _getBoundingBox( const TextFieldPtr & _node, mt::box2f * _bb ) override;
    };
}
