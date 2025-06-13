#pragma once

#include "Interface/LayoutInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutElementSetter
        : public LayoutElementSetterInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutElementGetter );

    public:
        PythonLayoutElementSetter();
        ~PythonLayoutElementSetter() override;

    protected:
        void onSetLayoutElementPosition( float _offset, float _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutElementSetter, LayoutElementSetterInterface> PythonLayoutElementSetterPtr;
    //////////////////////////////////////////////////////////////////////////
}