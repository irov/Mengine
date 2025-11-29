#import "AppleDetail.h"
#import "AppleString.h"
#import "AppleFactorablePtr.h"

#import "Environment/Apple/AppleBundle.h"

#include "Kernel/Assertion.h"

@implementation AppleDetail

+ (NSString * _Nonnull)NSIdToString:(id _Nonnull)value {
    NSString * value_string = [NSString stringWithFormat:@"%@", value];
    
    return value_string;
}

+ (NSString * _Nonnull)getMessageFromNSError:(NSError * _Nonnull)error {
    NSString * message = [NSString stringWithFormat:@"[Error %ld Description: %@ Failure reason: %@ Recovery suggestion: %@"
        , [error code]
        , [error description]
        , [error localizedFailureReason]
        , [error localizedRecoverySuggestion]
    ];
    
    return message;
}

+ (NSInteger)getTimestamp {
    NSTimeInterval time = [[NSDate date] timeIntervalSince1970];
    
    NSInteger millis = (NSInteger)(time * 1000.0);
    
    return millis;
}

+ (NSInteger)getSecureRandomInteger {
    NSInteger randomNumber;
    size_t size = sizeof(randomNumber);
    
    if (SecRandomCopyBytes(kSecRandomDefault, size, (uint8_t *)&randomNumber) != 0) {
        return 0;
    }
    
    if (randomNumber == 0) {
        randomNumber = 1;
    } else if (randomNumber < 0) {
        randomNumber = -randomNumber;
    }       
    
    return randomNumber;
}

+ (NSString * _Nonnull)getRandomHexString:(NSUInteger)length {
    MENGINE_ASSERTION_FATAL(length < 2048, "length must be less than 2048");
    
    NSUInteger lengthBytes = (length + 1) / 2;
    uint8_t randomBytes[1028];
    if (SecRandomCopyBytes(kSecRandomDefault, lengthBytes, randomBytes) != 0) {
        return @"";
    }
    
    NSUInteger capacity = (length + 1) / 2 * 2;
    
    NSMutableString * randomHexString = [NSMutableString stringWithCapacity:capacity];
    
    for (NSUInteger i = 0; i != lengthBytes; ++i) {
        [randomHexString appendFormat:@"%02X", randomBytes[i]];
    }
    
    NSString * randomHexStringTrim = [randomHexString substringToIndex:length];
    
    return randomHexStringTrim;
}

static volatile BOOL OPERATION_QUEUES_WORKING = YES;

+ (NSOperationQueue *)userInteractiveOperationQueue {
    static NSOperationQueue *queue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = [[NSOperationQueue alloc] init];
        NSString * bundleId = [AppleBundle getIdentifier];
        queue.name = [NSString stringWithFormat:@"%@.queue.user.interactive", bundleId];
        queue.qualityOfService = NSQualityOfServiceUserInteractive;
    });
    return queue;
}

+ (NSOperationQueue *)userInitiatedOperationQueue {
    static NSOperationQueue *queue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = [[NSOperationQueue alloc] init];
        NSString * bundleId = [AppleBundle getIdentifier];
        queue.name = [NSString stringWithFormat:@"%@.queue.user.initiated", bundleId];
        queue.qualityOfService = NSQualityOfServiceUserInitiated;
    });
    return queue;
}

+ (NSOperationQueue *)utilityOperationQueue {
    static NSOperationQueue *queue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = [[NSOperationQueue alloc] init];
        NSString * bundleId = [AppleBundle getIdentifier];
        queue.name = [NSString stringWithFormat:@"%@.queue.utility", bundleId];
        queue.qualityOfService = NSQualityOfServiceUtility;
    });
    return queue;
}

+ (NSOperationQueue *)backgroundOperationQueue {
    static NSOperationQueue *queue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = [[NSOperationQueue alloc] init];
        NSString * bundleId = [AppleBundle getIdentifier];
        queue.name = [NSString stringWithFormat:@"%@.queue.background", bundleId];
        queue.qualityOfService = NSQualityOfServiceBackground;
    });
    return queue;
}

+ (NSOperationQueue *)defaultOperationQueue {
    static NSOperationQueue *queue = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = [[NSOperationQueue alloc] init];
        NSString * bundleId = [AppleBundle getIdentifier];
        queue.name = [NSString stringWithFormat:@"%@.queue.default", bundleId];
        queue.qualityOfService = NSQualityOfServiceDefault;
    });
    return queue;
}

+ (void)addMainQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[NSOperationQueue mainQueue] addOperation:operation];
}

+ (void)addMainQueueOperation:(dispatch_block_t _Nonnull)block afterSeconds:(NSTimeInterval)seconds {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(seconds * NSEC_PER_SEC));
    
    dispatch_after(time, dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [AppleDetail addMainQueueOperation:block];
    });
}

+ (void)addDefaultQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[AppleDetail defaultOperationQueue] addOperation:operation];
}

+ (void)addBackgroundQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[AppleDetail backgroundOperationQueue] addOperation:operation];
}

+ (void)addUtilityQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[AppleDetail utilityOperationQueue] addOperation:operation];
}

+ (void)addUserInitiatedQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[AppleDetail userInitiatedOperationQueue] addOperation:operation];
}

+ (void)addUserInteractiveQueueOperation:(dispatch_block_t _Nonnull)block {
    if (OPERATION_QUEUES_WORKING == NO) {
        return;
    }
    
    NSBlockOperation * operation = [NSBlockOperation blockOperationWithBlock:block];
    
    [[AppleDetail userInteractiveOperationQueue] addOperation:operation];
}

+ (void)cancelAllQueueOperations {
    OPERATION_QUEUES_WORKING = NO;
    
    [[AppleDetail defaultOperationQueue] cancelAllOperations];
    [[AppleDetail backgroundOperationQueue] cancelAllOperations];
    [[AppleDetail utilityOperationQueue] cancelAllOperations];
    [[AppleDetail userInitiatedOperationQueue] cancelAllOperations];
    [[AppleDetail userInteractiveOperationQueue] cancelAllOperations];
    
    [[NSOperationQueue mainQueue] cancelAllOperations];
}

+ (BOOL)hasOption:(NSString *)value {
    NSString * hyphen_value = [NSString stringWithFormat:@"-%@", value];
    
    if ([[[NSProcessInfo processInfo] arguments] containsObject:hyphen_value] == YES) {
        return YES;
    }
    
    NSString * double_hyphen_value = [NSString stringWithFormat:@"--%@", value];
    
    if ([[[NSProcessInfo processInfo] arguments] containsObject:double_hyphen_value] == YES) {
        return YES;
    }
    
    return NO;
}

+ (BOOL)isValidJSONString:(NSString *)value {
    if (value == nil) {
        return NO;
    }
    
    NSData *data = [value dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error = nil;
    
    [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    
    if (error != nil) {
        return NO;
    }
    
    return YES;
}

+ (BOOL)getParamsFromJSON:(NSString * _Nonnull)_in outParams:(Mengine::Params * const _Nonnull)_out {
    if ([_in length] == 0) {
        return NO;
    }
    
    NSData * data = [_in dataUsingEncoding:NSUTF8StringEncoding];
    
    NSError * error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    
    if( error != nil ) {
        return NO;
    }
    
    NSDictionary * jsonDictionary = (NSDictionary *)jsonObject;
    
    [AppleDetail getParamsFromNSDictionary:jsonDictionary outParams:_out];
    
    return YES;
}

+ (void)visitParameters:(NSDictionary<NSString *, id> * _Nonnull)parameters
                forBool:(void (^ _Nonnull)(NSString * _Nonnull key, BOOL value))forBool
             forInteger:(void (^ _Nonnull)(NSString * _Nonnull key, int64_t value))forInteger
              forDouble:(void (^ _Nonnull)(NSString * _Nonnull key, double value))forDouble
              forString:(void (^ _Nonnull)(NSString * _Nonnull key, NSString * _Nonnull value))forString
                forNull:(void (^ _Nonnull)(NSString * _Nonnull key))forNull
             forUnknown:(void (^ _Nonnull)(NSString * _Nonnull key, id _Nonnull value))forUnknown {
    if (parameters == nil) {
        return;
    }
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    for (NSString * key in parameters) {
        id value = [parameters objectForKey:key];
        
        if (value == nil) {
            forNull(key);
        } else if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if (valueTypeId == boolenTypeId) {
                BOOL b = [value boolValue];
                
                forBool(key, b);
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
                        
                        forInteger(key, n);
                    } break;
                    case kCFNumberFloat32Type:
                    case kCFNumberFloat64Type:
                    case kCFNumberFloatType:
                    case kCFNumberDoubleType: {
                        double d = [value doubleValue];
                        
                        forDouble(key, d);
                    } break;
                    case kCFNumberCFIndexType:
                    case kCFNumberNSIntegerType:
                    case kCFNumberCGFloatType: {
                        double d = [value doubleValue];
                        
                        forDouble(key, d);
                    } break;
                }
            } else {
                forUnknown(key, value);
            }
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            NSString * s = (NSString *)value;
            
            forString(key, s);
        } else if ([value isKindOfClass:[NSNull class]]) {
            forNull(key);
        } else {
            forUnknown(key, value);
        }
    }
}

+ (void)visitValues:(NSSet<id> * _Nonnull)values
            forBool:(void (^ _Nonnull)(BOOL value))forBool
         forInteger:(void (^ _Nonnull)(int64_t value))forInteger
          forDouble:(void (^ _Nonnull)(double value))forDouble
          forString:(void (^ _Nonnull)(NSString * _Nonnull value))forString
            forNull:(void (^ _Nonnull)(void))forNull
         forUnknown:(void (^ _Nonnull)(id _Nonnull value))forUnknown {
    if (values == nil) {
        return;
    }
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    for (id value in values) {
        if (value == nil || [value isKindOfClass:[NSNull class]]) {
            forNull();
        } else if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if (valueTypeId == boolenTypeId) {
                BOOL b = [value boolValue];
                
                forBool(b);
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
                        
                        forInteger(n);
                    } break;
                    case kCFNumberFloat32Type:
                    case kCFNumberFloat64Type:
                    case kCFNumberFloatType:
                    case kCFNumberDoubleType:
                    case kCFNumberCFIndexType:
                    case kCFNumberNSIntegerType:
                    case kCFNumberCGFloatType: {
                        double d = [value doubleValue];
                        
                        forDouble(d);
                    } break;
                }
            } else {
                forUnknown(value);
            }
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            NSString * s = (NSString *)value;
            
            forString(s);
        } else {
            forUnknown(value);
        }
    }
}

+ (void)visitValues:(NSArray<id> * _Nonnull)values
            forBool:(void (^ _Nonnull)(BOOL value))forBool
         forInteger:(void (^ _Nonnull)(int64_t value))forInteger
          forDouble:(void (^ _Nonnull)(double value))forDouble
          forString:(void (^ _Nonnull)(NSString * _Nonnull value))forString
            forNull:(void (^ _Nonnull)(void))forNull
         forUnknown:(void (^ _Nonnull)(id _Nonnull value))forUnknown {
    if (values == nil) {
        return;
    }
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    for (id value in values) {
        if (value == nil || [value isKindOfClass:[NSNull class]]) {
            forNull();
        } else if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if (valueTypeId == boolenTypeId) {
                BOOL b = [value boolValue];
                
                forBool(b);
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
                        
                        forInteger(n);
                    } break;
                    case kCFNumberFloat32Type:
                    case kCFNumberFloat64Type:
                    case kCFNumberFloatType:
                    case kCFNumberDoubleType:
                    case kCFNumberCFIndexType:
                    case kCFNumberNSIntegerType:
                    case kCFNumberCGFloatType: {
                        double d = [value doubleValue];
                        
                        forDouble(d);
                    } break;
                }
            } else {
                forUnknown(value);
            }
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            NSString * s = (NSString *)value;
            
            forString(s);
        } else {
            forUnknown(value);
        }
    }
}

+ (void)getParamsFromNSDictionary:(NSDictionary *)_in outParams:(Mengine::Params * const)_out {
    if (_in == nil) {
        return;
    }
    
    [AppleDetail visitParameters:_in forBool:^(NSString * key, BOOL value) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        Mengine::ParamVariant variant = Mengine::ParamBool(value);
        
        _out->emplace(key_cstr, variant);
    } forInteger:^(NSString * key, int64_t value) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        Mengine::ParamVariant variant = Mengine::ParamInteger(value);
        
        _out->emplace(key_cstr, variant);
    } forDouble:^(NSString * key, double value) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        Mengine::ParamVariant variant = Mengine::ParamDouble(value);
        
        _out->emplace(key_cstr, variant);
    } forString:^(NSString * key, NSString * value) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        Mengine::ConstString s = [AppleString NSStringToConstString:value];
        Mengine::ParamVariant variant = Mengine::ParamConstString(s);
        
        _out->emplace(key_cstr, variant);
    } forNull:^(NSString * key) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        Mengine::ParamVariant variant = Mengine::ParamNull();
        
        _out->emplace(key_cstr, variant);
    } forUnknown:^(NSString * key, id value) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        const Mengine::Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
        Mengine::ParamVariant variant = Mengine::ParamString(value_str);
        
        _out->emplace(key_cstr, variant);
    }];
}

+ (NSDictionary *)getNSDictionaryFromParams:(const Mengine::Params &) _params {
    NSMutableDictionary * dictionary = [NSMutableDictionary dictionary];
    
    for( auto && [key, value] : _params ) {
        NSString * ns_key = [AppleString NSStringFromConstString:key];
        
        Mengine::Helper::visit( value
            , [dictionary, ns_key]( const Mengine::ParamNull & _element ) {
                [dictionary setObject:[NSNull null]
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamBool & _element ) {
                [dictionary setObject:[NSNumber numberWithBool:_element]
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamInteger & _element ) {
                [dictionary setObject:[NSNumber numberWithLongLong:_element]
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamDouble & _element ) {
                [dictionary setObject:[NSNumber numberWithDouble:_element]
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamString & _element ) {
                NSString * ns_element = [AppleString NSStringFromString:_element];
            
                [dictionary setObject:ns_element
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamWString & _element ) {
                NSString * ns_element = [AppleString NSStringFromUnicode:_element];
            
                [dictionary setObject:ns_element
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamConstString & _element ) {
                NSString * ns_element = [AppleString NSStringFromConstString:_element];
                
                [dictionary setObject:ns_element
                               forKey:ns_key];
            }, [dictionary, ns_key]( const Mengine::ParamFactorablePtr & _element ) {
                AppleFactorablePtr * ns_element = [[AppleFactorablePtr alloc] initWithValue:_element];
                
                [dictionary setObject:ns_element
                               forKey:ns_key];
            } );
    }
    
    return dictionary;
}

+ (void)getVectorStringFromNSArray:(NSArray<NSString *> *) _in outVectorString:(Mengine::VectorString *const) _out {
    for (NSString * string in _in) {
        Mengine::String cstr = [AppleString NSStringToString:string];
        
        _out->emplace_back( cstr );
    }
}

+ (NSArray<NSString *> *)getNSArrayFromVectorString:(const Mengine::VectorString &) _strings {
    NSMutableArray<NSString *> * array = [NSMutableArray array];
    
    for (const Mengine::String & str : _strings) {
        NSString * string = [AppleString NSStringFromString:str];
        
        [array addObject:string];
    }
    
    return array;
}

+ (void)getVectorConstStringFromNSArray:(NSArray<NSString *> *) _in outVectorConstString:(Mengine::VectorConstString *const) _out {
    for (NSString * string in _in) {
        Mengine::ConstString cstr = [AppleString NSStringToConstString:string];
        
        _out->emplace_back( cstr );
    }
}

+ (NSArray<NSString *> *)getNSArrayFromVectorConstString:(const Mengine::VectorConstString &) _strings {
    NSMutableArray<NSString *> * array = [NSMutableArray array];
    
    for (const Mengine::ConstString & cstr : _strings) {
        NSString * string = [AppleString NSStringFromConstString:cstr];
        
        [array addObject:string];
    }
    
    return array;
}

+ (void)getDataFromNSData:(NSData *) _in outData:(Mengine::Data *const) _out {
    const uint8_t * bytes = (const uint8_t *)[_in bytes];
    NSUInteger length = [_in length];
    
    _out->insert( _out->end(), bytes, bytes + length );
}

+ (NSData *)getNSDataFromData:(const Mengine::Data &) _data {
    const Mengine::Data::value_type * bytes = _data.data();
    Mengine::Data::size_type size = _data.size();
    
    NSData * data = [NSData dataWithBytes:bytes length:size];
    
    return data;
}

+ (void)raisePureVirtualMethodException:(Class)klass selector:(SEL)selector {
    @throw [NSException exceptionWithName:NSInternalInconsistencyException
                                   reason:[NSString stringWithFormat:@"You must override %@.%@ in a subclass", NSStringFromClass(klass), NSStringFromSelector(selector)]
                                 userInfo:nil];
}

@end
