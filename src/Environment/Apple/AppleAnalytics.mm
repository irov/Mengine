#import "AppleAnalytics.h"

#include "Interface/AnalyticsServiceInterface.h"

#include "Environment/Apple/AppleString.h"

@implementation AppleAnalytics

+ (void)name:(NSString*)eventName params:(NSDictionary<NSString*, id> *)params {
    Mengine::AnalyticsEventBuilderInterfacePtr builder = ANALYTICS_SERVICE()
        ->buildEvent( Mengine::Helper::NSStringToConstString(eventName) );
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    for (NSString* name in params) {
        id value = params[name];
        
        if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if( valueTypeId == boolenTypeId ) {
                BOOL b = (BOOL)value;
                
            } else if ( valueTypeId == numberTypeId ) {
                NSNumber * n = (NSNumber *)value;
                
            } else {
                return;
            }
            
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            NSString * s = (NSString *)value;
            
        } else {
            
        }
    }
    
    builder->log();
}

@end
