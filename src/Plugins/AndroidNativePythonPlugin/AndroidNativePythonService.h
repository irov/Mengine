#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Python/PythonIncluder.h"

#include "AndroidNativePythonInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"
#include "Kernel/Pair.h"

namespace Mengine
{
    class AndroidNativePythonService
        : public ServiceBase<AndroidNativePythonServiceInterface>
    {
    public:
        AndroidNativePythonService();
        ~AndroidNativePythonService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int32_t androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int64_t androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        double androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidJSONObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;

    protected:
        bool getAndroidMethod( MengineJNIEnvThread * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jplugin, jmethodID * const _jmethodId ) const;

    protected:
        pybind::kernel_interface * m_kernel;
    };
}
