#pragma once

#include "Interface/InputMousePositionProviderInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Arrow.h"

#include "math/vec2.h"

namespace Mengine
{
    class ArrowInputMousePositionProvider
        : public Factorable
        , public InputMousePositionProviderInterface
    {
    public:
        ArrowInputMousePositionProvider( Arrow * _arrow );
        ~ArrowInputMousePositionProvider() override;

    protected:
        void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position, float _pressure ) override;

    protected:
        Arrow * m_arrow;
    };
}
