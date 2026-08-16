#pragma once

#include "Interface/LayoutBoxInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutBoxElementGetter
        : public LayoutBoxElementGetterInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutBoxElementGetter );

    public:
        PythonLayoutBoxElementGetter();
        ~PythonLayoutBoxElementGetter() override;

    protected:
        bool onGetLayoutBoxElementSize( float * const _size ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutBoxElementGetter, LayoutBoxElementGetterInterface> PythonLayoutBoxElementGetterPtr;
    //////////////////////////////////////////////////////////////////////////
}
