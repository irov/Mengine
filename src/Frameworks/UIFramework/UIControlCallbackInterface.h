#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIFloatValueCallbackInterface
        : public Mixin
    {
    public:
        virtual void onUIFloatValue( float _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIFloatValueCallbackInterface> UIFloatValueCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIIndexValueCallbackInterface
        : public Mixin
    {
    public:
        virtual void onUIIndexValue( uint32_t _index ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIIndexValueCallbackInterface> UIIndexValueCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIBoolValueCallbackInterface
        : public Mixin
    {
    public:
        virtual void onUIBoolValue( bool _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIBoolValueCallbackInterface> UIBoolValueCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UIActionCallbackInterface
        : public Mixin
    {
    public:
        virtual void onUIAction() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UIActionCallbackInterface> UIActionCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
