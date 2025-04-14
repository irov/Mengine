#include "AndroidAnalyticsEventProvider.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidFragmentHelper.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidAnalyticsEventProvider::AndroidAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidAnalyticsEventProvider::~AndroidAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        const ConstString & eventName = _event->getName();

        jobject eventName_jobject = Helper::AndroidMakeJObjectString( jenv, eventName );

        Timestamp eventTimestamp = _event->getTimestamp();

        jobject bases_jobject = Helper::AndroidMakeJObjectHashMap( jenv, 0 );

        uint32_t countParameters = _event->getCountParameters();

        jobject parameters_jobject = Helper::AndroidMakeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [parameters_jobject, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
            {
                jobject name_jvalue = Helper::AndroidMakeJObjectString(jenv, _name);

                EAnalyticsEventParameterType parameterType = _parameter->getType();

                jobject parameter_jobject = nullptr;

                switch( parameterType )
                {
                    case EAEPT_BOOLEAN:
                    {
                        AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                        bool parameter_value = parameter_boolean->resolveValue();

                        parameter_jobject = Helper::AndroidMakeJObjectBoolean( jenv, parameter_value );
                    }break;
                    case EAEPT_INTEGER:
                    {
                        AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                        int64_t parameter_value = parameter_integer->resolveValue();

                        parameter_jobject = Helper::AndroidMakeJObjectLong( jenv, parameter_value );
                    }break;
                    case EAEPT_DOUBLE:
                    {
                        AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                        double parameter_value = parameter_double->resolveValue();

                        parameter_jobject = Helper::AndroidMakeJObjectDouble( jenv, parameter_value );
                    }break;
                    case EAEPT_STRING:
                    {
                        AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                        const String & parameter_value = parameter_string->resolveValue();

                        parameter_jobject = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                    }break;
                    case EAEPT_CONSTSTRING:
                    {
                        AnalyticsEventParameterConstStringInterfacePtr parameter_conststring = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                        const ConstString & parameter_value = parameter_conststring->resolveValue();

                        parameter_jobject = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                    }break;
                }

                Helper::AndroidPutJObjectMap( jenv, parameters_jobject, name_jvalue, parameter_jobject );

                jenv->DeleteLocalRef( parameter_jobject );
            });

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsEvent", "(Ljava/lang/String;JLjava/util/Map;Ljava/util/Map;)V", eventName_jobject, (jlong)eventTimestamp, bases_jobject, parameters_jobject );

        jenv->DeleteLocalRef( eventName_jobject );
        jenv->DeleteLocalRef( bases_jobject );
        jenv->DeleteLocalRef( parameters_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        jobject screenType_jobject = Helper::AndroidMakeJObjectString( jenv, _screenType );
        jobject screenName_jobject = Helper::AndroidMakeJObjectString( jenv, _screenName );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsScreenView", "(Ljava/lang/String;Ljava/lang/String;)V", screenType_jobject, screenName_jobject );

        jenv->DeleteLocalRef( screenType_jobject );
        jenv->DeleteLocalRef( screenName_jobject );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAnalyticsEventProvider::onAnalyticsFlush()
    {
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsFlush", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
}