#pragma once

#include "Interface/LayoutBoxInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutBoxSizer
        : public LayoutBoxSizerInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutBoxSizer );

    public:
        PythonLayoutBoxSizer();
        ~PythonLayoutBoxSizer() override;

    protected:
        bool onGetLayoutBoxSize( mt::vec2f * const _size ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutBoxSizer, LayoutBoxSizerInterface> PythonLayoutBoxSizerPtr;
    //////////////////////////////////////////////////////////////////////////
}
