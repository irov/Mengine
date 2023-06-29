#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AndroidNativePythonServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidNativePythonService" )

    public:
        virtual void addAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args ) = 0;
        virtual void removeAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) = 0;

    public:
        virtual void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual int32_t androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual int64_t androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual double androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual PyObject * androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;
        virtual PyObject * androidObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const = 0;

    public:
        virtual void waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_NATIVEPYTHON_SERVICE()\
    ((AndroidNativePythonServiceInterface *)SERVICE_GET(Mengine::AndroidNativePythonServiceInterface))
//////////////////////////////////////////////////////////////////////////