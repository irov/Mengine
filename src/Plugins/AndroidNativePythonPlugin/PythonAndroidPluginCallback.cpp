#include "PythonAndroidPluginCallback.h"

#include "AndroidNativePythonHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidPluginCallback::PythonAndroidPluginCallback( pybind::kernel_interface * _kernel, const pybind::object & _cb, const pybind::args & _args )
        : m_kernel( _kernel )
        , m_cb( _cb )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidPluginCallback::~PythonAndroidPluginCallback()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonAndroidPluginCallback::invoke( JNIEnv * _jenv, jobjectArray _args )
    {
        uint32_t cb_args_size = m_kernel->tuple_size( m_args );

        jsize args_size = Mengine_JNI_GetArrayLength( _jenv, _args );

        PyObject * py_args = m_kernel->tuple_new( args_size + cb_args_size );

        for( jsize index = 0; index != args_size; ++index )
        {
            jobject jobject_element = Mengine_JNI_GetObjectArrayElement( _jenv, _args, index );

            PyObject * py_arg = Helper::androidNativePythonMakePyObject( m_kernel, _jenv, jobject_element, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_FATAL( py_arg != nullptr, "android plugin method '%s' invalid arg"
                , m_cb.repr().c_str()
            );

            m_kernel->tuple_setitem( py_args, index, py_arg );

            m_kernel->decref( py_arg );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_element );
        }

        for( uint32_t index = 0; index != cb_args_size; ++index )
        {
            PyObject * cb_arg = m_args[index];

            m_kernel->tuple_setitem( py_args, args_size + index, cb_arg );
        }

        pybind::object py_result = m_cb.call_native( pybind::tuple( m_kernel, py_args, pybind::borrowed ) );

        MENGINE_ASSERTION_FATAL( py_result != nullptr, "android plugin method '%s' invalid call"
            , m_cb.repr().c_str()
        );

        m_kernel->decref( py_args );
    }
    //////////////////////////////////////////////////////////////////////////
}