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
        virtual void pythonMethod( const String & _plugin, const String & _method, jobjectArray _args ) = 0;
        virtual void addPlugin( const String & _name, jobject _plugin ) = 0;
        virtual void activateSemaphore( const String & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonEventHandlerInterface> AndroidNativePythonEventHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}