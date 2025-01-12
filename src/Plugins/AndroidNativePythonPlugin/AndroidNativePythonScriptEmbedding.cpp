#include "AndroidNativePythonScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "PythonAndroidSemaphoreListener.h"
#include "AndroidNativePythonInterface.h"
#include "AndroidNativePythonCallback.h"

#include "Kernel/ScriptablePrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ///////////////////////////////////////////////////////////////////////
        static pybind::object AndroidNativePythonService_addAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object cb = ANDROID_NATIVEPYTHON_SERVICE()
                ->addAndroidCallback( _plugin, _method, _cb, _args );

            return cb;
        }
        ///////////////////////////////////////////////////////////////////////
        static void AndroidNativePythonService_removeAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
        {
            ANDROID_NATIVEPYTHON_SERVICE()
                ->removeAndroidCallback( _plugin, _method, _cb );
        }
        ///////////////////////////////////////////////////////////////////////
        static void AndroidNativePythonService_androidMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            ANDROID_NATIVEPYTHON_SERVICE()
                ->androidMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static bool AndroidNativePythonService_androidBooleanMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidBooleanMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static int32_t AndroidNativePythonService_androidIntegerMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidIntegerMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static int64_t AndroidNativePythonService_androidLongMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidLongMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static float AndroidNativePythonService_androidFloatMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidFloatMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static double AndroidNativePythonService_androidDoubleMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidDoubleMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static PyObject * AndroidNativePythonService_androidStringMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidStringMethod( _plugin, _method, _args );
        }
        ///////////////////////////////////////////////////////////////////////
        static PyObject * AndroidNativePythonService_androidObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidObjectMethod( _plugin, _method, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidNativePythonService_waitSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            AndroidSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAndroidSemaphoreListener>( MENGINE_DOCUMENT_PYBIND, _cb, _args );

            ANDROID_NATIVEPYTHON_SERVICE()
                ->waitSemaphore( _name, listener );
        }
        ///////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonScriptEmbedding::AndroidNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonScriptEmbedding::~AndroidNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_args( _kernel, "setAndroidCallback", &Detail::AndroidNativePythonService_addAndroidCallback );
        pybind::def_function_args( _kernel, "addAndroidCallback", &Detail::AndroidNativePythonService_addAndroidCallback );
        pybind::def_function( _kernel, "removeAndroidCallback", &Detail::AndroidNativePythonService_removeAndroidCallback );
        pybind::def_function_args( _kernel, "androidMethod", &Detail::AndroidNativePythonService_androidMethod );
        pybind::def_function_args( _kernel, "androidBooleanMethod", &Detail::AndroidNativePythonService_androidBooleanMethod );
        pybind::def_function_args( _kernel, "androidIntegerMethod", &Detail::AndroidNativePythonService_androidIntegerMethod );
        pybind::def_function_args( _kernel, "androidLongMethod", &Detail::AndroidNativePythonService_androidLongMethod );
        pybind::def_function_args( _kernel, "androidFloatMethod", &Detail::AndroidNativePythonService_androidFloatMethod );
        pybind::def_function_args( _kernel, "androidDoubleMethod", &Detail::AndroidNativePythonService_androidDoubleMethod );
        pybind::def_function_args( _kernel, "androidStringMethod", &Detail::AndroidNativePythonService_androidStringMethod );
        pybind::def_function_args( _kernel, "androidObjectMethod", &Detail::AndroidNativePythonService_androidObjectMethod );

        pybind::interface_<AndroidNativePythonCallback, pybind::bases<Scriptable>>( _kernel, "AndroidNativePythonCallback" )
            .def_call( &AndroidNativePythonCallback::call )
            ;

        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::AndroidNativePythonService_waitSemaphore );

        Helper::registerScriptWrapping<AndroidNativePythonCallback>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "AndroidNativePython" ), STRINGIZE_STRING_LOCAL( "AndroidNativePythonCallback" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<AndroidNativePythonCallback, 32>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<AndroidNativePythonCallback>();

        Helper::unregisterScriptWrapping<AndroidNativePythonCallback>();

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "AndroidNativePython" ), STRINGIZE_STRING_LOCAL( "AndroidNativePythonCallback" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

