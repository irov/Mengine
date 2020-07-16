#pragma once

#include "Engine/TextField.h"

#include "Kernel/BaseDebuggerBoundingBox.h"

namespace Mengine
{
    class TextFieldDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<TextFieldPtr>
        , public Factorable
    {
    public:
        TextFieldDebuggerBoundingBox();
        ~TextFieldDebuggerBoundingBox() override;

    public:
        bool _getBoundingBox( const TextFieldPtr & _node, mt::box2f * const _bb ) const override;
    };
}
