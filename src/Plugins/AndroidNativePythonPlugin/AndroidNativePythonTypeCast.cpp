#include "AndroidNativePythonTypeCast.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Python/PythonDocument.h"

#include "AndroidNativePythonHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_specialized<jobject>::operator () ( kernel_interface * _kernel, const jobject & _value ) const
    {
        JNIEnv * jenv = Mengine::Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        PyObject * py_value = Mengine::Helper::androidNativePythonMakePyObject( _kernel, jenv, _value, MENGINE_DOCUMENT_PYTHON );

        if( py_value == nullptr )
        {
            pybind::throw_exception( "invalid convert jobject to Python" );
        }

        return py_value;
    }
    //////////////////////////////////////////////////////////////////////////
}
