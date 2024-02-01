#include "AndroidProxyLogger.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/AssertionMemoryPanic.h"
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
    void AndroidProxyLogger::_log( const LoggerMessage & _message )
    {
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.data, _message.size );

        if( (_message.filter & Mengine::LFILTER_ANDROID) == Mengine::LFILTER_ANDROID )
        {
            return;
        }

        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jstring category_jstring = jenv->NewStringUTF( _message.category );

        ELoggerLevel level = _message.level;

        const Char * data_value = _message.data;
        size_t data_size = _message.size;

        Char msg[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_MEMCPY( msg, data_value, data_size * sizeof(Char) );
        msg[data_size] = '\0';

        jstring data_jstring = jenv->NewStringUTF( msg );

        Helper::AndroidCallVoidApplicationMethod( jenv, "onMengineLogger", "(ILjava/lang/String;Ljava/lang/String;)V", level, category_jstring, data_jstring );

        jenv->DeleteLocalRef( category_jstring );
        jenv->DeleteLocalRef( data_jstring );
    }
    //////////////////////////////////////////////////////////////////////////
}
