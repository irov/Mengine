#pragma once

#include "Interface/LayoutInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutElementGetter
        : public LayoutElementGetterInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutElementGetter );

    public:
        PythonLayoutElementGetter();
        ~PythonLayoutElementGetter() override;

    protected:
        bool onGetLayoutElementSize( float * const _size ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutElementGetter, LayoutElementGetterInterface> PythonLayoutElementGetterPtr;
    //////////////////////////////////////////////////////////////////////////
}