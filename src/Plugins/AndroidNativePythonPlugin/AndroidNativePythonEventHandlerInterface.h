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
        virtual void pythonMethod( const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) = 0;
        virtual void addPlugin( const ConstString & _name, jobject _plugin ) = 0;
        virtual void activateSemaphore( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonEventHandlerInterface> AndroidNativePythonEventHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}