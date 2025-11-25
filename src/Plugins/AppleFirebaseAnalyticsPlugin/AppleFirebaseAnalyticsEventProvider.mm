#include "AppleFirebaseAnalyticsEventProvider.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"
#include "Kernel/AnalyticsHelper.h"

#include "Config/StdAlgorithm.h"

#import <FirebaseAnalytics/FirebaseAnalytics.h>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsEventProvider::AppleFirebaseAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsEventProvider::~AppleFirebaseAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        [FIRAnalytics logEventWithName:kFIREventScreenView
                            parameters:@{kFIRParameterScreenClass: [AppleString NSStringFromConstString:_screenType],
                                         kFIRParameterScreenName: [AppleString NSStringFromConstString:_screenName]}];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();
        
        uint32_t countParameters = _event->getCountParameters();
        
        if( countParameters > 25 )
        {
            return;
        }
        
        NSMutableDictionary<NSString *, id> * firebase_parameters = [[NSMutableDictionary alloc] init];
        
        _event->foreachParameters( [firebase_parameters]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            ConstString::size_type name_size = _name.size();
            
            if( name_size > 40 )
            {
                return;
            }
            
            const Char * name_str = _name.c_str();

            Helper::visitAnalyticsParameter( _parameter
                , [firebase_parameters, name_str]( bool _value )
            {
                [firebase_parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [firebase_parameters, name_str]( int64_t _value )
            {
                [firebase_parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [firebase_parameters, name_str]( double _value )
            {
                [firebase_parameters setValue:@(_value) forKey:@(name_str)];
            }
                , [firebase_parameters, name_str]( const String & _value )
            {
                String::size_type parameter_value_size = _value.size();
                
                if( parameter_value_size > 100 )
                {
                    return;
                }
                
                const Char * parameter_value_str = _value.c_str();

                [firebase_parameters setValue:@(parameter_value_str) forKey:@(name_str)];
            }
                , [firebase_parameters, name_str]( const ConstString & _value )
            {
                ConstString::size_type parameter_value_size = _value.size();
                
                if( parameter_value_size > 100 )
                {
                    return;
                }
                
                const Char * parameter_value_str = _value.c_str();

                [firebase_parameters setValue:@(parameter_value_str) forKey:@(name_str)];
            } );
        } );
        
        [FIRAnalytics logEventWithName:@(eventName_str) 
                            parameters:firebase_parameters];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsEventProvider::onAnalyticsFlush()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
}
