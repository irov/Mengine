#include "AndroidLinearAccelerationPlugin.h"

#include "Core/ModuleFactory.h"

#include "Logger/Logger.h"

#include "pybind/pybind.hpp"
#include "math/vec3.h"

#include <android/native_window_jni.h>

extern "C" {
#define MENGINE_JAVA_PREFIX                             org_Mengine_Build
#define CONCAT1(prefix, class, function)                CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                Java_ ## prefix ## _ ## class ## _ ## function
#define MENGINE_JAVA_INTERFACE(function)                CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)

    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( onNativeLinearAccel )(
        JNIEnv *env, jclass jcls,
        jfloat x, jfloat y, jfloat z);

    float fLastLinearAccelerometer[3] = { 0.f, 0.f, 0.f };

    /* Accelerometer */
    JNIEXPORT void JNICALL MENGINE_JAVA_INTERFACE( onNativeLinearAccel )(
        JNIEnv *env, jclass jcls,
        jfloat x, jfloat y, jfloat z) {
        fLastLinearAccelerometer[0] = x;
        fLastLinearAccelerometer[1] = y;
        fLastLinearAccelerometer[2] = z;
    }
}

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidLinearAcceleration, Mengine::AndroidLinearAccelerationPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLinearAccelerationPlugin::AndroidLinearAccelerationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLinearAccelerationPlugin::~AndroidLinearAccelerationPlugin()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool AndroidLinearAccelerationPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * getLinearAccelerationValue()
    {
        PyObject * pyvalue = pybind::build_value( "(fff)"
            , fLastLinearAccelerometer[0]
            , fLastLinearAccelerometer[1]
            , fLastLinearAccelerometer[2]
        );

        return pyvalue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidLinearAccelerationPlugin::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function( kernel, "getLinearAccelerationValue", &getLinearAccelerationValue );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLinearAccelerationPlugin::_finalize()
    {
    }
}
