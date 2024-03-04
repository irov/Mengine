#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEventation.h"
#include "Environment/Python/PythonIncluder.h"

#include "AndroidNativePythonInterface.h"
#include "AndroidNativePythonEventHandlerInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"
#include "Kernel/Pair.h"

namespace Mengine
{
    class AndroidNativePythonService
        : public ServiceBase<AndroidNativePythonServiceInterface>
        , public AndroidNativePythonEventHandlerInterface
    {
    public:
        AndroidNativePythonService();
        ~AndroidNativePythonService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        typedef AndroidEventation<AndroidNativePythonEventHandlerInterface> PythonEventation;
        typedef IntrusivePtr<PythonEventation> PythonEventationPtr;
        typedef typename PythonEventation::LambdaEventHandler LambdaPythonEventHandler;

    public:
        void addCommand( const LambdaPythonEventHandler & _command );

    public:
        bool hasPythonMethod( const ConstString & _plugin, const ConstString & _method ) const override;
        void callPythonMethod( const ConstString & _plugin, const ConstString & _method, jobjectArray _args ) const override;
        void addPlugin( const ConstString & _name, jobject _plugin ) override;

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
        void waitSemaphore( const ConstString & _name, const AndroidFunctorVoidInterfacePtr & _listener ) override;

    protected:
        bool getAndroidMethod( JNIEnv * _jenv, const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const;

    protected:
        void notifyChangeSceneComplete_( const ScenePtr & _scene );
        void notifyRemoveSceneComplete_();

    protected:
        pybind::kernel_interface * m_kernel;

        FactoryInterfacePtr m_factoryAndroidNativePythonFunctorVoid;
        FactoryInterfacePtr m_factoryAndroidNativePythonFunctorBoolean;

        struct AndroidPythonCallbackDesc
        {
            pybind::object cb;
            pybind::args args;
        };

        typedef Vector<AndroidPythonCallbackDesc> VectorAndroidPythonCallbacks;

        typedef Map<Pair<ConstString, ConstString>, VectorAndroidPythonCallbacks> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        ThreadMutexInterfacePtr m_callbacksMutex;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;

        PythonEventationPtr m_eventation;
    };
}
