#pragma once

#include "Interface/LayoutBoxInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonLayoutBoxElementSetter
        : public LayoutBoxElementSetterInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonLayoutBoxElementSetter );

    public:
        PythonLayoutBoxElementSetter();
        ~PythonLayoutBoxElementSetter() override;

    protected:
        void onSetLayoutBoxElementPosition( const mt::vec2f & _offset, const mt::vec2f & _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonLayoutBoxElementSetter, LayoutBoxElementSetterInterface> PythonLayoutBoxElementSetterPtr;
    //////////////////////////////////////////////////////////////////////////
}
