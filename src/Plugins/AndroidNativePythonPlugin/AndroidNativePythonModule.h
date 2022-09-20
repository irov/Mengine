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
    class PythonEventHandlerInterface
        : public Mixin
    {
    public:
        virtual void pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args ) = 0;

        virtual void addPlugin( const String & _name, const jobject & _plugin ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonEventHandlerInterface> PythonEventHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativePythonModule
        : public ModuleBase
        , public PythonEventHandlerInterface
    {
        DECLARE_FACTORABLE( AndroidNativePythonModule );

    public:
        typedef AndroidEventation<PythonEventHandlerInterface> PythonEventation;
        typedef IntrusivePtr<PythonEventation> PythonEventationPtr;
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

    protected:
        PyObject * getPythonAttribute( jobject obj );

    public:
        void pythonMethod( const String & _plugin, const String & _method, int32_t _id, jobjectArray _args ) override;
        void addPlugin( const String & _name, const jobject & _plugin ) override;

    public:
        void setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb );
        bool androidResponse( int32_t _id, const pybind::object & _result ) const;
        void androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;
        bool androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;
        int32_t androidInteger32Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;
        int64_t androidInteger64Method( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;
        float androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;
        String androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args ) const;

    protected:
        bool getAndroidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args, const Char * _retType, jvalue * const _jargs, jobject * const _jfree, uint32_t * const _freeCount, jobject * const _jplugin, jmethodID * const _jmethodId ) const;

    protected:
        JNIEnv * m_jenv;

        jclass m_jclass_Object;
        jclass m_jclass_Boolean;
        jclass m_jclass_Character;
        jclass m_jclass_Integer;
        jclass m_jclass_Long;
        jclass m_jclass_Float;
        jclass m_jclass_Double;
        jclass m_jclass_String;
        jclass m_jclass_ArrayList;

        pybind::kernel_interface * m_kernel;

        typedef Map<Pair<ConstString, ConstString>, pybind::object> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        typedef Map<ConstString, jobject> MapAndroidPlugins;
        MapAndroidPlugins m_plugins;

        PythonEventationPtr m_eventation;
    };
    //////////////////////////////////////////////////////////////////////////
}
