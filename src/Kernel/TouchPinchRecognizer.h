#pragma once

#include "Kernel/InputEvent.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TouchPinchRecognizer
    {
    public:
        TouchPinchRecognizer();

    public:
        bool begin( const mt::vec2f & _first, const mt::vec2f & _second, const Resolution & _resolution );
        bool isActive() const;
        EInputGesturePhase update( const mt::vec2f & _first, const mt::vec2f & _second, const Resolution & _resolution, float * const _scale );
        void reset();

    protected:
        float m_distance;
    };
    //////////////////////////////////////////////////////////////////////////
}
