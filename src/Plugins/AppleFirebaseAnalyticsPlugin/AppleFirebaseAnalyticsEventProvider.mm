#include "AppleFirebaseAnalyticsEventProvider.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"

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
            
            EAnalyticsEventParameterType parameterType = _parameter->getType();

            switch( parameterType )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                    bool parameter_value = parameter_boolean->resolveValue();

                    [firebase_parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                    int64_t parameter_value = parameter_integer->resolveValue();

                    [firebase_parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                    double parameter_value = parameter_double->resolveValue();

                    [firebase_parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                    const String & parameter_value = parameter_string->resolveValue();
                    
                    String::size_type parameter_value_size = parameter_value.size();
                    
                    if( parameter_value_size > 100 )
                    {
                        return;
                    }
                    
                    const Char * parameter_value_str = parameter_value.c_str();

                    [firebase_parameters setValue:@(parameter_value_str) forKey:@(name_str)];
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterfacePtr parameter_string = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                    const ConstString & parameter_value = parameter_string->resolveValue();
                    
                    ConstString::size_type parameter_value_size = parameter_value.size();
                    
                    if( parameter_value_size > 100 )
                    {
                        return;
                    }
                    
                    const Char * parameter_value_str = parameter_value.c_str();

                    [firebase_parameters setValue:@(parameter_value_str) forKey:@(name_str)];
                }break;
            }
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
