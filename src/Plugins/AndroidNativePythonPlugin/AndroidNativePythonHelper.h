#pragma once

#include "Interface/DocumentInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        MENGINE_NODISCARD PyObject * androidNativePythonMakePyObject( pybind::kernel_interface * _kernel, JNIEnv * _jenv, jobject _obj, const DocumentInterfacePtr & _doc );
        MENGINE_NODISCARD jobject androidNativePythonListMakeJavaObject( JNIEnv * _jenv, const pybind::list & _list );
        MENGINE_NODISCARD jobject androidNativePythonDictMakeJavaObject( JNIEnv * _jenv, const pybind::dict & _dict );
        MENGINE_NODISCARD jobject androidNativePythonMakeJavaObject( JNIEnv * _jenv, const pybind::object & _obj );
    }
}