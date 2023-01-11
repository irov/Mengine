#include "AndroidProxyLogger.h"

#include "Environment/Android/AndroidUtils.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidProxyLogger::AndroidProxyLogger()
        : m_jenv( nullptr )
        , m_jobjectMengineActivity( nullptr )
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
    void AndroidProxyLogger::setJClassMengineActivity( jclass _jclassMengineActivity )
    {
        m_jclassMengineActivity = _jclassMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass AndroidProxyLogger::getJClassMengineActivity() const
    {
        return m_jclassMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::setJObjectMengineActivity( jobject _jobjectMengineActivity )
    {
        m_jobjectMengineActivity = _jobjectMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidProxyLogger::getJObjectMengineActivity() const
    {
        return m_jobjectMengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidProxyLogger::log( const LoggerMessage & _message )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        static jmethodID jmethodID_onMengineLogger = jenv->GetMethodID(m_jclassMengineActivity, "onMengineLogger", "(Ljava/lang/String;IIILjava/lang/String;)V" );

        MENGINE_ASSERTION(jmethodID_onMengineLogger != nullptr, "invalid get android method 'onMengineLogger'" );

        const Char * category_str = _message.category.c_str();

        jstring category_jstring = jenv->NewStringUTF( category_str );

        ELoggerLevel level = _message.level;
        uint32_t filter = _message.filter;
        uint32_t color = _message.color;

        const Char * data_value = _message.data;
        size_t data_size = _message.size;

        MENGINE_MEMCPY( m_loggerMessage, data_value, data_size * sizeof(Char) );
        m_loggerMessage[data_size] = '\0';

        jstring data_jstring = jenv->NewStringUTF( m_loggerMessage );

        jenv->CallVoidMethod(m_jobjectMengineActivity, jmethodID_onMengineLogger, category_jstring, level, filter, color, data_jstring);

        jenv->DeleteLocalRef( category_jstring );
        jenv->DeleteLocalRef( data_jstring );
    }
    //////////////////////////////////////////////////////////////////////////
}
