#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Mixin.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativePythonEventHandlerInterface
        : public Mixin
    {
    public:
        virtual bool hasPythonMethod( const ConstString & _plugin, const ConstString & _method ) const = 0;
        virtual void callPythonMethod( const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const = 0;
        virtual void addPlugin( const ConstString & _name, jobject _plugin ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonEventHandlerInterface> AndroidNativePythonEventHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}