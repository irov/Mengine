#include "AppleAmplitudeAnalyticsEventProvider.h"

#include "Kernel/AnalyticsHelper.h"

#import <Amplitude/Amplitude.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAmplitudeAnalyticsEventProvider::AppleAmplitudeAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAmplitudeAnalyticsEventProvider::~AppleAmplitudeAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAmplitudeAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();

        NSMutableDictionary<NSString *, id> * properties = [[NSMutableDictionary alloc] init];

        _event->foreachParameters( [properties]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();

            Helper::visitAnalyticsParameter( _parameter
                , [properties, name_str]( bool _value )
            {
                [properties setValue:@(_value) forKey:@(name_str)];
            }
                , [properties, name_str]( int64_t _value )
            {
                [properties setValue:@(_value) forKey:@(name_str)];
            }
                , [properties, name_str]( double _value )
            {
                [properties setValue:@(_value) forKey:@(name_str)];
            }
                , [properties, name_str]( const String & _value )
            {
                const Char * value_str = _value.c_str();

                [properties setValue:@(value_str) forKey:@(name_str)];
            }
                , [properties, name_str]( const ConstString & _value )
            {
                const Char * value_str = _value.c_str();

                [properties setValue:@(value_str) forKey:@(name_str)];
            } );
        } );

        [[Amplitude instance] logEvent:@(eventName_str) withEventProperties:properties];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAmplitudeAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        const Char * screenType_str = _screenType.c_str();
        const Char * screenName_str = _screenName.c_str();

        [[Amplitude instance] logEvent:@"screen_view"
                   withEventProperties:@{@"screen_type": @(screenType_str),
                                         @"screen_name": @(screenName_str)}];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAmplitudeAnalyticsEventProvider::onAnalyticsFlush()
    {
        [[Amplitude instance] uploadEvents];
    }
    //////////////////////////////////////////////////////////////////////////
}

