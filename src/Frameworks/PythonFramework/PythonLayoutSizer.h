#pragma once

#include "Interface/LayoutInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutSizer
        : public LayoutSizerInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutSizer );

    public:
        PythonLayoutSizer();
        ~PythonLayoutSizer() override;

    protected:
        bool onGetLayoutSize( float * const _size ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutSizer, LayoutSizerInterface> PythonLayoutSizerPtr;
    //////////////////////////////////////////////////////////////////////////
}