#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Map.h"
#include "Kernel/Pair.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEventHandler
            : public Mixin
    {
    public:
        virtual void pythonMethod( const String & _plugin, const String & _method, const String & _args ) = 0;
        virtual void addPlugin( const String & _name, const jobject & _plugin ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEventHandler> PythonEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativePythonModule
        : public ModuleBase
        , public PythonEventHandler
    {
        DECLARE_FACTORABLE( AndroidNativePythonModule );

    public:
        typedef AndroidEventation<PythonEventHandler> PythonEventation;
        typedef typename PythonEventation::LambdaEventHandler LambdaPythonEventHandler;

    public:
        AndroidNativePythonModule();
        ~AndroidNativePythonModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaPythonEventHandler & _command );

    public:
        void pythonMethod( const String & _plugin, const String & _method, const String & _args ) override;
        void addPlugin( const String & _name, const jobject & _plugin ) override;

    public:
        void setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb );
        bool androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;

    protected:
        JNIEnv * m_jenv;
        jclass m_jclassActivity;

        pybind::kernel_interface * m_kernel;

        pybind::dict m_globals;

        typedef Map<Pair<ConstString, ConstString>, pybind::object> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;

        mutable PythonEventation m_eventation;
    };
    //////////////////////////////////////////////////////////////////////////
}
