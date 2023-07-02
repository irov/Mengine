#pragma once

#include "Interface/DocumentInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        PyObject * androidNativePythonMakePyObject( pybind::kernel_interface * _kernel, JNIEnv * _jenv, jobject _obj, const DocumentInterfacePtr & _doc );
        jobject androidNativePythonMakeJavaObject( JNIEnv * _jenv, pybind::kernel_interface * _kernel, PyObject * _obj );
    }
}