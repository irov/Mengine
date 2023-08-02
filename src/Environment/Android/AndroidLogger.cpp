#include "AndroidLogger.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::~AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::_log( const LoggerMessage & _message )
    {
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.category, MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( _message.data, _message.size );

        if( (_message.filter & Mengine::LFILTER_ANDROID) == Mengine::LFILTER_ANDROID )
        {
            return;
        }

        ELoggerLevel level = _message.level;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        if( _message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};

            const Char * file = _message.file;
            int32_t line = _message.line;
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( file, line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );

            MENGINE_STRCAT( message, functionstamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char date[256] = {'\0'};

            const PlatformDateTime & dateTime = _message.dateTime;
            size_t dateSize = Helper::makeLoggerShortDate( dateTime, "[%02u:%02u:%02u:%04u]", date, 0, 256 );

            MENGINE_STRCAT( message, date );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );

            MENGINE_STRCAT( message, threadstamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_CATEGORYSTAMP )
        {
            const Char * category = _message.category;
            
            MENGINE_STRCAT( message, "[" );
            MENGINE_STRCAT( message, category );
            MENGINE_STRCAT( message, "]" );
            MENGINE_STRCAT( message, " " );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        size_t message_stamplen = MENGINE_STRLEN( message );

        if( size + message_stamplen >= MENGINE_LOGGER_MAX_MESSAGE )
        {
            size = MENGINE_LOGGER_MAX_MESSAGE - message_stamplen - 1;
        }

        MENGINE_STRNCAT( message, data, size );

        JNIEnv * env = Mengine_JNI_GetEnv();

        jclass jclass_MengineLog = env->FindClass( "org/Mengine/Base/MengineLog" );

        jmethodID jclass_MengineLog_logLevel = env->GetStaticMethodID( jclass_MengineLog, "logLevel", "(ILjava/lang/String;Ljava/lang/String;)V" );

        jint jint_level = (jint)level;
        jstring jstring_Mengine = env->NewStringUTF( "Mengine" );
        jstring jstring_message = env->NewStringUTF( message );

        env->CallStaticVoidMethod( jclass_MengineLog, jclass_MengineLog_logLevel, jint_level, jstring_Mengine, jstring_message );

        env->DeleteLocalRef( jstring_Mengine );
        env->DeleteLocalRef( jstring_message );

        env->DeleteLocalRef( jclass_MengineLog );
    }
    //////////////////////////////////////////////////////////////////////////
}
