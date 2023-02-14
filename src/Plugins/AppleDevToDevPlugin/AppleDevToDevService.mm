#include "AppleDevToDevService.h"

#include "Environment/iOS/iOSUtils.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdString.h"

#import <DTDAnalytics/DTDAnalytics-Swift.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleDevToDevService, Mengine::AppleDevToDevService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevService::AppleDevToDevService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevService::~AppleDevToDevService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevService::_initializeService()
    {
        [DTDAnalytics trackingAvailabilityHandler:^(BOOL value) {
            LOGGER_MESSAGE( "initialized has been finished [%s]"
                , (value == TRUE ? "SUCCESSFUL" : "FAILED")
            );
            
            //Empty
        }];
        
        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevService::_finalizeService()
    {
        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevService::sendEvent( NSString * _eventName, NSDictionary<NSString *, id> * _parameters )
    {
        LOGGER_MESSAGE( "sendEvent name: %s parameters: %s"
            , [ _eventName UTF8String]
            , [[NSString stringWithFormat:@"%@", _parameters] UTF8String]
        );

        DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];

        for (NSString * key in _parameters)
        {
            id value = _parameters[key];
            
            //NSString * s = [NSString stringWithFormat:@"%@", [value class]];
            
            //TODO!!!
            [devtodev_parameters addString:key value:@"TODO"];
        }

        [DTDAnalytics customEvent:_eventName withParameters:devtodev_parameters];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevService::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {        
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();
        
        DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];
        
        _event->foreachParameters( [devtodev_parameters]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();
            
            EAnalyticsEventParameterType parameterType = _parameter->getType();

            switch( parameterType )
            {
            case EAEPT_BOOLEAN:
                {
                    AnalyticsEventParameterBooleanInterfacePtr parameter_boolean = AnalyticsEventParameterBooleanInterfacePtr::from( _parameter );
                    bool parameter_value = parameter_boolean->resolveValue();

                    [devtodev_parameters addBool:@(name_str) value:@(parameter_value)];
                }break;
            case EAEPT_INTEGER:
                {
                    AnalyticsEventParameterIntegerInterfacePtr parameter_integer = AnalyticsEventParameterIntegerInterfacePtr::from( _parameter );
                    int64_t parameter_value = parameter_integer->resolveValue();

                    [devtodev_parameters addInt:@(name_str) value:parameter_value];
                }break;
            case EAEPT_DOUBLE:
                {
                    AnalyticsEventParameterDoubleInterfacePtr parameter_double = AnalyticsEventParameterDoubleInterfacePtr::from( _parameter );
                    double parameter_value = parameter_double->resolveValue();

                    [devtodev_parameters addDouble:@(name_str) value:parameter_value];
                }break;
            case EAEPT_STRING:
                {
                    AnalyticsEventParameterStringInterfacePtr parameter_string = AnalyticsEventParameterStringInterfacePtr::from( _parameter );
                    const String & parameter_value = parameter_string->resolveValue();
                    
                    const Char * parameter_value_str = parameter_value.c_str();

                    [devtodev_parameters addString:@(name_str) value:@(parameter_value_str)];
                }break;
            }
        } );
        
        [DTDAnalytics customEvent:@(eventName_str) withParameters:devtodev_parameters];
    }
    //////////////////////////////////////////////////////////////////////////
}
