#include "AndroidProxyLogger.h"

#include "Interface/AndroidEnvironmentServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"

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
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jmethodID jmethodID_onMengineLogger = ANDROID_ENVIRONMENT_SERVICE()
            ->getApplicationMethodID( jenv, "onMengineLogger", "(Ljava/lang/String;IIILjava/lang/String;)V" );

        jstring category_jstring = jenv->NewStringUTF( _message.category );

        ELoggerLevel level = _message.level;
        uint32_t filter = _message.filter;
        uint32_t color = _message.color;

        const Char * data_value = _message.data;
        size_t data_size = _message.size;

        MENGINE_MEMCPY( m_loggerMessage, data_value, data_size * sizeof(Char) );
        m_loggerMessage[data_size] = '\0';

        jstring data_jstring = jenv->NewStringUTF( m_loggerMessage );

        ANDROID_ENVIRONMENT_SERVICE()
                ->callVoidApplicationMethod(jenv, jmethodID_onMengineLogger, category_jstring, level, filter, color, data_jstring);

        jenv->DeleteLocalRef( category_jstring );
        jenv->DeleteLocalRef( data_jstring );
    }
    //////////////////////////////////////////////////////////////////////////
}
