#import "AppleAnalytics.h"

#include "Interface/AnalyticsServiceInterface.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/DocumentHelper.h"

@implementation AppleAnalytics

+ (void)event:(NSString*)eventName params:(NSDictionary<NSString*, id> *)params {
    Mengine::AnalyticsEventBuilderInterfacePtr builder = ANALYTICS_SERVICE()
        ->buildEvent( Mengine::Helper::NSStringToConstString(eventName), MENGINE_DOCUMENT_FUNCTION );
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    for (NSString * key in params) {
        id value = params[key];
        
        Mengine::ConstString key_cstr = Mengine::Helper::NSStringToConstString(key);
        
        if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if (valueTypeId == boolenTypeId) {
                bool b = [value boolValue];
                
                builder->addParameterBoolean( key_cstr, b );
            } else if (valueTypeId == numberTypeId) {
                CFNumberType numberType = CFNumberGetType((__bridge CFNumberRef)value);
                
                switch (numberType) {
                    case kCFNumberSInt8Type:
                    case kCFNumberSInt16Type:
                    case kCFNumberSInt32Type:
                    case kCFNumberSInt64Type:
                    case kCFNumberCharType:
                    case kCFNumberShortType:
                    case kCFNumberIntType:
                    case kCFNumberLongType:
                    case kCFNumberLongLongType: {
                        int64_t n = [value longLongValue];
                        
                        builder->addParameterInteger( key_cstr, n );
                    }break;
                        
                    case kCFNumberFloat32Type:
                    case kCFNumberFloat64Type:
                    case kCFNumberFloatType:
                    case kCFNumberDoubleType: {
                        double d = [value doubleValue];
                        
                        builder->addParameterDouble( key_cstr, d );
                    }break;
                    case kCFNumberCFIndexType:
                    case kCFNumberNSIntegerType:
                    case kCFNumberCGFloatType: {
                        return;
                    }break;
                }
            } else {
                return;
            }
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            Mengine::ConstString s = Mengine::Helper::NSStringToConstString(value);
            
            builder->addParameterConstString( key_cstr, s );
        } else if ([value isKindOfClass:[NSDictionary class]] == YES) {
            //ToDo
            
        } else {
            return;
        }
    }
    
    builder->log();
}

@end
