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
#define MENGINE_ACTIVITY_JAVA_INTERFACE(function)                CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define MENGINE_SURFACE_JAVA_INTERFACE(function)                CONCAT1(MENGINE_JAVA_PREFIX, MengineSurface, function)

    float fLastAccelerometer[3] = { 0.f, 0.f, 0.f };
    float fLastLinearAccelerometer[3] = { 0.f, 0.f, 0.f };

    bool fLastAccelerometerGravityInitialize = false;
    float fLastAccelerometerGravity[3] = { 0.f, 0.f, 0.f };
    float fLastAccelerometerMotion[3] = { 0.f, 0.f, 0.f };

    JNIEXPORT void JNICALL MENGINE_SURFACE_JAVA_INTERFACE( onNativeAccel )(
            JNIEnv *env, jclass jcls,
            jfloat x, jfloat y, jfloat z) {

        fLastAccelerometer[0] = x;
        fLastAccelerometer[1] = y;
        fLastAccelerometer[2] = z;

        if( fLastAccelerometerGravityInitialize == false ) {
            fLastAccelerometerGravityInitialize = true;
            fLastAccelerometerGravity[0] = fLastAccelerometer[0];
            fLastAccelerometerGravity[1] = fLastAccelerometer[1];
            fLastAccelerometerGravity[2] = fLastAccelerometer[2];
        } else {
            fLastAccelerometerGravity[0] =
                    0.1 * fLastAccelerometer[0] + 0.9 * fLastAccelerometerGravity[0];
            fLastAccelerometerGravity[1] =
                    0.1 * fLastAccelerometer[1] + 0.9 * fLastAccelerometerGravity[1];
            fLastAccelerometerGravity[2] =
                    0.1 * fLastAccelerometer[2] + 0.9 * fLastAccelerometerGravity[2];
        }

        fLastAccelerometerMotion[0] = fLastAccelerometer[0] - fLastAccelerometerGravity[0];
        fLastAccelerometerMotion[1] = fLastAccelerometer[1] - fLastAccelerometerGravity[1];
        fLastAccelerometerMotion[2] = fLastAccelerometer[2] - fLastAccelerometerGravity[2];
    }

    /* Accelerometer */
    JNIEXPORT void JNICALL MENGINE_SURFACE_JAVA_INTERFACE( onNativeLinearAccel )(
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
    static PyObject * androidGetLinearAcceleration()
    {
        PyObject * pyvalue = pybind::build_value( "(fff)"
            , fLastLinearAccelerometer[0]
            , fLastLinearAccelerometer[1]
            , fLastLinearAccelerometer[2]
        );

        return pyvalue;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * androidGetAccelerationMotion()
    {
        PyObject * pyvalue = pybind::build_value( "(fff)"
                , fLastAccelerometerMotion[0]
                , fLastAccelerometerMotion[1]
                , fLastAccelerometerMotion[2]
        );

        return pyvalue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidLinearAccelerationPlugin::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function( kernel, "androidGetLinearAcceleration", &androidGetLinearAcceleration );
        pybind::def_function( kernel, "androidGetAccelerationMotion", &androidGetAccelerationMotion );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLinearAccelerationPlugin::_finalize()
    {
    }
}
