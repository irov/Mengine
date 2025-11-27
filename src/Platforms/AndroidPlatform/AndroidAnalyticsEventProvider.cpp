#include "AndroidAnalyticsEventProvider.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidFragmentHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AnalyticsHelper.h"

namespace Mengine
{
    namespace Detail
    {
        static const Char * getAndroidAnalyticsCategoryEnumName( EAnalyticsEventCategory _category )
        {
            switch( _category )
            {
            case AEEC_SYSTEM:
                return "MengineAnalyticsEventCategory_System";
            default:
                return "MengineAnalyticsEventCategory_Custom";
            }
        }
    }

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

                jobject jobject_parameter = nullptr;

                Helper::visitAnalyticsParameter( _parameter
                    , [jenv, &jobject_parameter]( bool _value )
                {
                    jobject_parameter = Helper::AndroidMakeJObjectBoolean( jenv, _value );
                }
                    , [jenv, &jobject_parameter]( int64_t _value )
                {
                    jobject_parameter = Helper::AndroidMakeJObjectLong( jenv, _value );
                }
                    , [jenv, &jobject_parameter]( double _value )
                {
                    jobject_parameter = Helper::AndroidMakeJObjectDouble( jenv, _value );
                }
                    , [jenv, &jobject_parameter]( const String & _value )
                {
                    jobject_parameter = Helper::AndroidMakeJObjectString( jenv, _value );
                }
                    , [jenv, &jobject_parameter]( const ConstString & _value )
                {
                    jobject_parameter = Helper::AndroidMakeJObjectString( jenv, _value );
                } );

                Helper::AndroidPutJObjectMap( jenv, jobject_parameters, jobject_name, jobject_parameter );

                Mengine_JNI_DeleteLocalRef( jenv, jobject_name );
                Mengine_JNI_DeleteLocalRef( jenv, jobject_parameter );
            });

        EAnalyticsEventCategory eventCategory = _event->getCategory();

        const Char * categoryEnumName = Detail::getAndroidAnalyticsCategoryEnumName( eventCategory );

        jobject jobject_category = Helper::AndroidGetJObjectEnum( jenv, "org/Mengine/Base/MengineAnalyticsEventCategory", categoryEnumName );

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
        JNIEnv * jenv = Mengine_JNI_GetEnv();

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
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( jenv == nullptr )
        {
            return;
        }

        Helper::AndroidCallVoidFragmentMethod( jenv, "MengineFragmentAnalytics", "analyticsFlush", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
}