#include "AndroidProxyLogger.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Error.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidProxyLogger::AndroidProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidProxyLogger::~AndroidProxyLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidProxyLogger::_initializeLogger()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::_finalizeLogger()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::log( const LoggerMessage & _message )
    {
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.data, _message.size );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            MENGINE_ERROR_FATAL("invalid get jenv");

            return;
        }

        jmethodID jmethodID_onMengineLogger = ANDROID_ENVIRONMENT_SERVICE()
            ->getApplicationMethodID( jenv, "onMengineLogger", "(Ljava/lang/String;IIILjava/lang/String;)V" );

        jstring category_jstring = jenv->NewStringUTF( _message.category );

        ELoggerLevel level = _message.level;
        uint32_t filter = _message.filter;
        uint32_t color = _message.color;

        const Char * data_value = _message.data;
        size_t data_size = _message.size;

        Char msg[MENGINE_LOGGER_MAX_MESSAGE];
        MENGINE_MEMCPY( msg, data_value, data_size * sizeof(Char) );
        msg[data_size] = '\0';

        jstring data_jstring = jenv->NewStringUTF( msg );

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidApplicationMethod(jenv, jmethodID_onMengineLogger, category_jstring, level, filter, color, data_jstring);

        jenv->DeleteLocalRef( category_jstring );
        jenv->DeleteLocalRef( data_jstring );
    }
    //////////////////////////////////////////////////////////////////////////
}
