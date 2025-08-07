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
        MengineJNIEnvThread * jenv = Mengine_JNI_GetEnvThread();

        if( jenv == nullptr )
        {
            return;
        }

        const ConstString & eventName = _event->getName();

        jobject jobject_eventName = Helper::AndroidMakeJObjectString( jenv, eventName );

        Timestamp eventTimestamp = _event->getTimestamp();

        jobject jobject_bases = Helper::AndroidMakeJObjectHashMap( jenv, 0 );

        uint32_t countParameters = _event->getCountParameters();

        jobject jobject_parameters = Helper::AndroidMakeJObjectHashMap( jenv, countParameters );

        _event->foreachParameters( [jobject_parameters, jenv]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
            {
                jobject jobject_name = Helper::AndroidMakeJObjectString( jenv, _name);

                EAnalyticsEventParameterType parameterType = _parameter->getType();

                jobject jobject_parameter = nullptr;

                switch( parameterType )
                {
                    case EAEPT_BOOLEAN:
                    {
                        AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                        bool parameter_value = parameter_boolean->resolveValue();

                        jobject_parameter = Helper::AndroidMakeJObjectBoolean( jenv, parameter_value );
                    }break;
                    case EAEPT_INTEGER:
                    {
                        AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                        int64_t parameter_value = parameter_integer->resolveValue();

                        jobject_parameter = Helper::AndroidMakeJObjectLong( jenv, parameter_value );
                    }break;
                    case EAEPT_DOUBLE:
                    {
                        AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                        double parameter_value = parameter_double->resolveValue();

                        jobject_parameter = Helper::AndroidMakeJObjectDouble( jenv, parameter_value );
                    }break;
                    case EAEPT_STRING:
                    {
                        AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                        const String & parameter_value = parameter_string->resolveValue();

                        jobject_parameter = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                    }break;
                    case EAEPT_CONSTSTRING:
                    {
                        AnalyticsEventParameterConstStringInterfacePtr parameter_conststring = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                        const ConstString & parameter_value = parameter_conststring->resolveValue();

                        jobject_parameter = Helper::AndroidMakeJObjectString( jenv, parameter_value );
                    }break;
                }

                Helper::AndroidPutJObjectMap( jenv, jobject_parameters, jobject_name, jobject_parameter );

                Mengine_JNI_DeleteLocalRef( jenv, jobject_name );
                Mengine_JNI_DeleteLocalRef( jenv, jobject_parameter );
            });

        jobject jobject_category = Helper::AndroidGetJObjectEnum( jenv, "org/Mengine/Base/MengineAnalyticsEventCategory", "MengineAnalyticsEventCategory_Custom" );

        jclass jclass_MengineAnalyticsEventParam = Helper::AndroidEnvFindClass( jenv, "org/Mengine/Base/MengineParamAnalyticsEvent" );

        jmethodID jmethod_MengineAnalyticsEventParam = Mengine_JNI_GetMethodID( jenv, jclass_MengineAnalyticsEventParam, "<init>", "(Lorg/Mengine/Base/MengineAnalyticsEventCategory;Ljava/lang/String;JLjava/util/Map;Ljava/util/Map;)V" );

        jobject jobject_event = Mengine_JNI_NewObject( jenv, jclass_MengineAnalyticsEventParam, jmethod_MengineAnalyticsEventParam, jobject_category, jobject_eventName, (jlong)eventTimestamp, jobject_bases, jobject_parameters );

        Helper::AndroidEnvExceptionCheck( jenv );

        Mengine_JNI_DeleteLocalRef( jenv, jclass_MengineAnalyticsEventParam );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_category );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_eventName );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_bases );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_parameters );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsEvent", "(Lorg/Mengine/Base/MengineParamAnalyticsEvent;)V", jobject_event );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        MengineJNIEnvThread * jenv = Mengine_JNI_GetEnvThread();

        if( jenv == nullptr )
        {
            return;
        }

        jobject jobject_ScreenType = Helper::AndroidMakeJObjectString( jenv, _screenType );
        jobject jobject_ScreenName = Helper::AndroidMakeJObjectString( jenv, _screenName );

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsScreenView", "(Ljava/lang/String;Ljava/lang/String;)V", jobject_ScreenType, jobject_ScreenName );

        Mengine_JNI_DeleteLocalRef( jenv, jobject_ScreenType );
        Mengine_JNI_DeleteLocalRef( jenv, jobject_ScreenName );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAnalyticsEventProvider::onAnalyticsFlush()
    {
        MengineJNIEnvThread * jenv = Mengine_JNI_GetEnvThread();

        if( jenv == nullptr )
        {
            return;
        }

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsFlush", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
}