#pragma once

#include "Interface/DocumentInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        MENGINE_NODISCARD PyObject * androidNativePythonMakePyObject( pybind::kernel_interface * _kernel, MengineJNIEnvThread * _jenv, jobject _obj, const DocumentInterfacePtr & _doc );
        MENGINE_NODISCARD jobject androidNativePythonListMakeJavaObject( MengineJNIEnvThread * _jenv, const pybind::list & _list );
        MENGINE_NODISCARD jobject androidNativePythonDictMakeJavaObject( MengineJNIEnvThread * _jenv, const pybind::dict & _dict );
        MENGINE_NODISCARD jobject androidNativePythonMakeJavaObject( MengineJNIEnvThread * _jenv, const pybind::object & _obj );
    }
}