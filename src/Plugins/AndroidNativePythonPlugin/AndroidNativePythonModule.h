#pragma once

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Map.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonEventHandler
            : public Mixin
    {
    public:
        virtual void pythonMethod( const String & _method, const String & _args ) = 0;
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
        void pythonMethod( const String & _method, const String & _args ) override;

    public:
        void setAndroidCallback( const ConstString & _method, const pybind::object & _cb );
        void androidMethod( const ConstString & _method, const pybind::args & _args ) const;

    protected:
        pybind::kernel_interface * m_kernel;

        pybind::dict m_globals;

        typedef Map<ConstString, pybind::object> MapAndroidCallbacks;
        MapAndroidCallbacks m_callbacks;

        PythonEventation m_eventation;
    };
    //////////////////////////////////////////////////////////////////////////
}
