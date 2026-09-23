#include "PythonAndroidSemaphoreListener.h"

#include "AndroidNativePythonTypeCast.h"

#include "Environment/Android/AndroidGlobalRef.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidSemaphoreListener::PythonAndroidSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
        : PythonCallbackProvider( _cb, _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidSemaphoreListener::~PythonAndroidSemaphoreListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonAndroidSemaphoreListener::invoke( JNIEnv * _jenv, jobject _value )
    {
        AndroidGlobalRefPtr value_gref = Helper::makeAndroidGlobalRef( _jenv, _value, MENGINE_DOCUMENT_FACTORABLE );

        if( value_gref == nullptr )
        {
            return;
        }

        PythonAndroidSemaphoreListenerPtr keep = PythonAndroidSemaphoreListenerPtr::from( this );

        Helper::dispatchMainThreadEvent( [keep, value_gref]() {
            jobject value = value_gref->getValue();

            keep->call_cb( value );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}