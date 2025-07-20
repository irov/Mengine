#include "iOSAnalyticsEventProvider.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/iOS/iOSAnalytics.h"

#include "Kernel/AssertionMemoryPanic.h"

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
        
        uint32_t countParameters = _event->getCountParameters();
        
        NSMutableDictionary<NSString *, id> * parameters = [[NSMutableDictionary alloc] init];
        
        _event->foreachParameters( [parameters]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();
            ConstString::size_type name_size = _name.size();
            
            EAnalyticsEventParameterType parameterType = _parameter->getType();

            switch( parameterType )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                    bool parameter_value = parameter_boolean->resolveValue();

                    [parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                    int64_t parameter_value = parameter_integer->resolveValue();

                    [parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                    double parameter_value = parameter_double->resolveValue();

                    [parameters setValue:@(parameter_value) forKey:@(name_str)];
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                    const String & parameter_value = parameter_string->resolveValue();
                    
                    String::size_type parameter_value_size = parameter_value.size();
                                        
                    const Char * parameter_value_str = parameter_value.c_str();

                    [parameters setValue:@(parameter_value_str) forKey:@(name_str)];
                }break;
            case EAEPT_CONSTSTRING:
                {
                    AnalyticsEventParameterConstStringInterfacePtr parameter_string = AnalyticsEventParameterConstStringInterfacePtr::from( _parameter );
                    const ConstString & parameter_value = parameter_string->resolveValue();
                    
                    ConstString::size_type parameter_value_size = parameter_value.size();
                                        
                    const Char * parameter_value_str = parameter_value.c_str();

                    [parameters setValue:@(parameter_value_str) forKey:@(name_str)];
                }break;
            }
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
