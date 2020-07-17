#pragma once

#include "Spine.h"

#include "Kernel/BaseDebuggerBoundingBox.h"

namespace Mengine
{
    class SpineDebuggerBoundingBox
        : public BaseDebuggerBoundingBox<SpinePtr>
        , public Factorable
    {
    public:
        SpineDebuggerBoundingBox();
        ~SpineDebuggerBoundingBox() override;

    public:
        bool _getBoundingBox( const SpinePtr & _node, mt::box2f * const _bb ) const override;
    };
}
