#include "AppleDevToDevAnalyticsEventProvider.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/AnalyticsHelper.h"

#include "Config/StdString.h"

#import <DTDAnalytics/DTDAnalytics-Swift.h>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevAnalyticsEventProvider::AppleDevToDevAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevAnalyticsEventProvider::~AppleDevToDevAnalyticsEventProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevAnalyticsEventProvider::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {        
        const ConstString & eventName = _event->getName();
        const Char * eventName_str = eventName.c_str();
        
        DTDCustomEventParameters * devtodev_parameters = [[DTDCustomEventParameters alloc] init];
        
        _event->foreachParameters( [devtodev_parameters]( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter )
        {
            const Char * name_str = _name.c_str();

            Helper::visitAnalyticsParameter( _parameter
                , [devtodev_parameters, name_str]( bool _value )
            {
                [devtodev_parameters addBool:@(name_str) value:@(_value)];
            }
                , [devtodev_parameters, name_str]( int64_t _value )
            {
                [devtodev_parameters addInt:@(name_str) value:_value];
            }
                , [devtodev_parameters, name_str]( double _value )
            {
                [devtodev_parameters addDouble:@(name_str) value:_value];
            }
                , [devtodev_parameters, name_str]( const String & _value )
            {
                const Char * parameter_value_str = _value.c_str();

                [devtodev_parameters addString:@(name_str) value:@(parameter_value_str)];
            }
                , [devtodev_parameters, name_str]( const ConstString & _value )
            {
                const Char * parameter_value_str = _value.c_str();

                [devtodev_parameters addString:@(name_str) value:@(parameter_value_str)];
            } );
        } );
        
        [DTDAnalytics customEvent:@(eventName_str) withParameters:devtodev_parameters];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevAnalyticsEventProvider::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        MENGINE_UNUSED( _screenType );
        MENGINE_UNUSED( _screenName );
        
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevAnalyticsEventProvider::onAnalyticsFlush()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
}
