#include "AndroidLogger.h"

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/AssertionUtf8.h"
#include "Kernel/AssertionMemoryPanic.h"
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
            MENGINE_UNUSED( functionstampSize );

            MENGINE_STRCAT( message, functionstamp );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char date[256] = {'\0'};

            const PlatformDateTime & dateTime = _message.dateTime;
            size_t dateSize = Helper::makeLoggerShortDate( dateTime, "[%02u:%02u:%02u:%04u]", date, 0, 256 );
            MENGINE_UNUSED( dateSize );

            MENGINE_STRCAT( message, date );
            MENGINE_STRCAT( message, " " );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            MENGINE_UNUSED( threadstampSize );

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

        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jclass jclass_MengineLog = jenv->FindClass( "android/util/Log" );

        if( jclass_MengineLog == nullptr )
        {
            return;
        }

        const Char * method = nullptr;

        switch( level )
        {
            case LM_SILENT:
                return;
            case LM_FATAL:
                method = "wtf";
                break;
            case LM_MESSAGE_RELEASE:
                method = "w";
                break;
            case LM_ERROR:
                method = "e";
                break;
            case LM_WARNING:
                method = "w";
                break;
            case LM_MESSAGE:
                method = "i";
                break;
            case LM_INFO:
                method = "i";
                break;
            case LM_DEBUG:
                method = "d";
                break;
            case LM_VERBOSE:
                method = "v";
                break;
            default:
                return;
        }

        jmethodID jclass_Log_method = jenv->GetStaticMethodID( jclass_MengineLog, method, "(Ljava/lang/String;Ljava/lang/String;)I" );

        if( jclass_Log_method == nullptr )
        {
            return;
        }

        jstring jstring_Mengine = jenv->NewStringUTF( "Mengine" );
        jstring jstring_message = jenv->NewStringUTF( message );

        jint result = jenv->CallStaticIntMethod( jclass_MengineLog, jclass_Log_method, jstring_Mengine, jstring_message );
        MENGINE_UNUSED( result );

        jenv->DeleteLocalRef( jstring_Mengine );
        jenv->DeleteLocalRef( jstring_message );

        jenv->DeleteLocalRef( jclass_MengineLog );
    }
    //////////////////////////////////////////////////////////////////////////
}
