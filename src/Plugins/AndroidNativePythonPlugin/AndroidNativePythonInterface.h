#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class AndroidNativePythonServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidNativePythonService" )

    public:
        virtual void setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) = 0;
        virtual bool androidResponse( JNIEnv * _jenv, int32_t _id, const pybind::object & _result ) const = 0;
        virtual void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual int32_t androidInteger32Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual int64_t androidInteger64Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual String androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual void waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_NATIVEPYTHON_SERVICE()\
    ((AndroidNativePythonServiceInterface *)SERVICE_GET(Mengine::AndroidNativePythonServiceInterface))
//////////////////////////////////////////////////////////////////////////