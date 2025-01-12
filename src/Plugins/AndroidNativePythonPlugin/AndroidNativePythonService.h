#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Android/AndroidIncluder.h"
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
        bool hasPythonMethod( const ConstString & _plugin, const ConstString & _method ) const override;
        void callPythonMethod( const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const override;
        void addPlugin( const ConstString & _name, jobject _jplugin ) override;
        jobject removePlugin( const ConstString & _name ) override;

    public:
        pybind::object addAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args ) override;
        void removeAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) override;

    public:
        void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int32_t androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        int64_t androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        double androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;
        PyObject * androidObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const override;

    public:
        void waitSemaphore( const ConstString & _name, const AndroidSemaphoreListenerInterfacePtr & _listener ) override;

    protected:
        jobject getAndroidPlugin( const ConstString & _plugin ) const;
        bool getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const;

    protected:
        pybind::kernel_interface * m_kernel;

        struct AndroidPythonCallbackDesc
        {
            pybind::object cb;
            pybind::args args;
        };

        typedef Vector<AndroidPythonCallbackDesc> VectorAndroidPythonCallbacks;

        typedef Map<Pair<ConstString, ConstString>, VectorAndroidPythonCallbacks> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        ThreadMutexInterfacePtr m_callbacksMutex;
        ThreadMutexInterfacePtr m_pluginsMutex;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;
    };
}
