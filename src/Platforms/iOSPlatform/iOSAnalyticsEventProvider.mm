#include "iOSAnalyticsEventProvider.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAnalytics.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AnalyticsHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSAnalyticsEventProvider::iOSAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAnalyticsEventProvider::~iOSAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();
        
        NSMutableDictionary<NSString *, id> * parameters = [[NSMutableDictionary alloc] init];
        
        _event->foreachParameters( [parameters]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();
            ConstString::size_type name_size = _name.size();
            
            MENGINE_UNUSED( name_size );

            Helper::visitAnalyticsParameter( _parameter
                , [parameters, name_str]( bool _value )
            {
                [parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [parameters, name_str]( int64_t _value )
            {
                [parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [parameters, name_str]( double _value )
            {
                [parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [parameters, name_str]( const String & _value )
            {
                const Char * parameter_value_str = _value.c_str();

                [parameters setValue:@(parameter_value_str) forKey:@(name_str)];
            }
                , [parameters, name_str]( const ConstString & _value )
            {
                const Char * parameter_value_str = _value.c_str();

                [parameters setValue:@(parameter_value_str) forKey:@(name_str)];
            } );
        } );
        
        [iOSAnalytics event:@(eventName_str) params:parameters];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        const Char * screenType_str = _screenType.c_str();
        const Char * screenName_str = _screenName.c_str();

        [iOSAnalytics screen:@(screenName_str) type:@(screenType_str)];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAnalyticsEventProvider::onAnalyticsFlush()
    {
        [iOSAnalytics flush];
    }
    //////////////////////////////////////////////////////////////////////////
}
