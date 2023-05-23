#include "AndroidNativePythonScriptEmbedding.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "AndroidNativePythonInterface.h"
#include "AndroidNativePythonFunctorVoid.h"
#include "AndroidNativePythonFunctorBoolean.h"

#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ///////////////////////////////////////////////////////////////////////
        static void AndroidNativePythonService_setAndroidCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args )
        {
            ANDROID_NATIVEPYTHON_SERVICE()
                ->setAndroidCallback( _plugin, _method, _cb, _args );
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
        ///////////////////////////////////////////////////////////////////////
        static void AndroidNativePythonService_waitAndroidSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            ANDROID_NATIVEPYTHON_SERVICE()
                ->waitAndroidSemaphore( _name, _cb, _args );
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
        pybind::def_function_args( _kernel, "setAndroidCallback", &Detail::AndroidNativePythonService_setAndroidCallback );
        pybind::def_function_args( _kernel, "androidMethod", &Detail::AndroidNativePythonService_androidMethod );
        pybind::def_function_args( _kernel, "androidBooleanMethod", &Detail::AndroidNativePythonService_androidBooleanMethod );
        pybind::def_function_args( _kernel, "androidIntegerMethod", &Detail::AndroidNativePythonService_androidIntegerMethod );
        pybind::def_function_args( _kernel, "androidLongMethod", &Detail::AndroidNativePythonService_androidLongMethod );
        pybind::def_function_args( _kernel, "androidFloatMethod", &Detail::AndroidNativePythonService_androidFloatMethod );
        pybind::def_function_args( _kernel, "androidDoubleMethod", &Detail::AndroidNativePythonService_androidDoubleMethod );
        pybind::def_function_args( _kernel, "androidStringMethod", &Detail::AndroidNativePythonService_androidStringMethod );
        pybind::def_function_args( _kernel, "androidObjectMethod", &Detail::AndroidNativePythonService_androidObjectMethod );

        pybind::def_function_args( _kernel, "waitAndroidSemaphore", &Detail::AndroidNativePythonService_waitAndroidSemaphore );

        pybind::interface_<AndroidNativePythonFunctor, pybind::bases<Scriptable>>( _kernel, "AndroidNativePythonFunctor" )
            ;

        pybind::interface_<AndroidNativePythonFunctorVoid, pybind::bases<AndroidNativePythonFunctor>>( _kernel, "AndroidNativePythonFunctorVoid" )
            .def( "call", &AndroidNativePythonFunctorVoid::call )
            ;

        pybind::interface_<AndroidNativePythonFunctorBoolean, pybind::bases<AndroidNativePythonFunctor>>( _kernel, "AndroidNativePythonFunctorBoolean" )
            .def( "call", &AndroidNativePythonFunctorBoolean::call )
            ;

        Helper::registerScriptWrapping<AndroidNativePythonFunctorVoid>( _kernel, STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorVoid" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<AndroidNativePythonFunctorBoolean>( _kernel, STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorBoolean" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<AndroidNativePythonFunctorVoid>();
        _kernel->remove_scope<AndroidNativePythonFunctorBoolean>();

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorVoid" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "AndroidNativePythonFunctorBoolean" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}

