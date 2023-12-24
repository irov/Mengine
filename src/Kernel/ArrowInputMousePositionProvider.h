#pragma once

#include "Interface/InputMousePositionProviderInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Arrow.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArrowInputMousePositionProvider
        : public InputMousePositionProviderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( ArrowInputMousePositionProvider );

    public:
        ArrowInputMousePositionProvider( Arrow * _arrow );
        ~ArrowInputMousePositionProvider() override;

    protected:
        void onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure ) override;

    protected:
        Arrow * m_arrow;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArrowInputMousePositionProvider, InputMousePositionProviderInterface> ArrowInputMousePositionProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}