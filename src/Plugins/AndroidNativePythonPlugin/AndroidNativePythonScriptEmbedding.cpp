#include "AndroidNativePythonScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Android/AndroidKernelServiceInterface.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "PythonAndroidSemaphoreListener.h"
#include "PythonAndroidPluginCallback.h"
#include "AndroidNativePythonInterface.h"
#include "AndroidNativePythonCallback.h"

#include "Kernel/ScriptablePrototypeGenerator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ///////////////////////////////////////////////////////////////////////
        static AndroidPluginCallbackInterfacePtr AndroidNativePythonService_addAndroidCallback( pybind::kernel_interface * _kernel, const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args )
        {
            AndroidPluginCallbackInterfacePtr callback = Helper::makeFactorableUnique<PythonAndroidPluginCallback>( MENGINE_DOCUMENT_PYBIND, _kernel, _cb, _args );

            ANDROID_KERNEL_SERVICE()
                ->addPluginCallback( _plugin, _method, callback );

            return callback;
        }
        ///////////////////////////////////////////////////////////////////////
        static void AndroidNativePythonService_removeAndroidCallback( const ConstString & _plugin, const ConstString & _method, const AndroidPluginCallbackInterfacePtr & _callback )
        {
            ANDROID_KERNEL_SERVICE()
                ->removePluginCallback( _plugin, _method, _callback );
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
        static PyObject * AndroidNativePythonService_androidJSONObjectMethod( const ConstString & _plugin, const ConstString & _method, const pybind::args & _args )
        {
            return ANDROID_NATIVEPYTHON_SERVICE()
                ->androidJSONObjectMethod( _plugin, _method, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        AndroidSemaphoreListenerInterfacePtr AndroidNativePythonService_waitSemaphore( const ConstString & _semaphore, const pybind::object & _cb, const pybind::args & _args )
        {
            AndroidSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAndroidSemaphoreListener>( MENGINE_DOCUMENT_PYBIND, _cb, _args );

            ANDROID_KERNEL_SERVICE()
                ->waitSemaphore( _semaphore, listener );

            return listener;
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
        pybind::def_function_kernel_args( _kernel, "setAndroidCallback", &Detail::AndroidNativePythonService_addAndroidCallback );
        pybind::def_function_kernel_args( _kernel, "addAndroidCallback", &Detail::AndroidNativePythonService_addAndroidCallback );
        pybind::def_function( _kernel, "removeAndroidCallback", &Detail::AndroidNativePythonService_removeAndroidCallback );
        pybind::def_function_args( _kernel, "androidMethod", &Detail::AndroidNativePythonService_androidMethod );
        pybind::def_function_args( _kernel, "androidBooleanMethod", &Detail::AndroidNativePythonService_androidBooleanMethod );
        pybind::def_function_args( _kernel, "androidIntegerMethod", &Detail::AndroidNativePythonService_androidIntegerMethod );
        pybind::def_function_args( _kernel, "androidLongMethod", &Detail::AndroidNativePythonService_androidLongMethod );
        pybind::def_function_args( _kernel, "androidFloatMethod", &Detail::AndroidNativePythonService_androidFloatMethod );
        pybind::def_function_args( _kernel, "androidDoubleMethod", &Detail::AndroidNativePythonService_androidDoubleMethod );
        pybind::def_function_args( _kernel, "androidStringMethod", &Detail::AndroidNativePythonService_androidStringMethod );
        pybind::def_function_args( _kernel, "androidObjectMethod", &Detail::AndroidNativePythonService_androidObjectMethod );
        pybind::def_function_args( _kernel, "androidJSONObjectMethod", &Detail::AndroidNativePythonService_androidJSONObjectMethod );

        pybind::interface_<AndroidPluginCallbackInterface, pybind::bases<Mixin>>( _kernel, "AndroidPluginCallbackInterface" )
            ;

        pybind::interface_<AndroidSemaphoreListenerInterface, pybind::bases<Mixin>>( _kernel, "AndroidSemaphoreListenerInterface" )
            ;

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

