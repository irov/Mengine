#import "AppleDetail.h"
#import "AppleString.h"

@implementation AppleDetail

+ (NSIDMessage)NSIdToString:(id _Nonnull) _value {
    const Mengine::Char * message_str = [[NSString stringWithFormat:@"%@", _value] UTF8String];

    return NSIDMessage( message_str );
}

+ (NSErrorMessage)getMessageFromNSError:(NSError *) _error {
    NSString * message = [NSString stringWithFormat:@"[Error %ld Description: %@ Failure reason: %@ Recovery suggestion: %@"
        , [_error code]
        , [_error description]
        , [_error localizedFailureReason]
        , [_error localizedRecoverySuggestion]
        ];

    const Mengine::Char * message_str = [message UTF8String];

    return NSErrorMessage( message_str );
}

+ (NSInteger)getCurrentTimeMillis {
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
    
    return randomNumber;
}

+ (NSString *)getRandomHexString:(NSInteger) _length {
    NSInteger lengthBytes = (_length + 1) / 2;
    uint8_t randomBytes[lengthBytes];
    if (SecRandomCopyBytes(kSecRandomDefault, lengthBytes, randomBytes) != 0) {
        return @"";
    }
    
    NSMutableString * randomHexString = [NSMutableString stringWithCapacity:(_length + 1) / 2 * 2];
    
    for (NSInteger i = 0; i != lengthBytes; ++i) {
        [randomHexString appendFormat:@"%02X", randomBytes[i]];
    }
    
    NSString * randomHexStringTrim = [randomHexString substringToIndex:_length];
    
    return randomHexStringTrim;
}

+ (BOOL)isValidJSONString:(NSString *) _value {
    if (_value == nil) {
        return NO;
    }
    
    NSData *data = [_value dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error = nil;
    
    [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    
    if (error != nil) {
        return NO;
    }
    
    return YES;
}

+ (BOOL)getParamsFromJSON:(NSString * _Nonnull) _in outParams:(Mengine::Params * const _Nonnull) _out {
    if ([_in length] == 0) {
        return NO;
    }
    
    NSData * data = [_in dataUsingEncoding:NSUTF8StringEncoding];
    
    NSError * error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    
    if( error != nil )
    {
        return NO;
    }
    
    NSDictionary * jsonDictionary = (NSDictionary *)jsonObject;
    
    [AppleDetail getParamsFromNSDictionary:jsonDictionary outParams:_out];
    
    return YES;
}

+ (void)getParamsFromNSDictionary:(NSDictionary *) _in outParams:(Mengine::Params * const) _out {
    if (_in == nil) {
        return;
    }
    
    CFTypeID boolenTypeId = CFBooleanGetTypeID();
    CFTypeID numberTypeId = CFNumberGetTypeID();
    
    [_in enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL * stop) {
        Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
        
        Mengine::ParamVariant variant;
        
        if ([value isKindOfClass:[NSNumber class]] == YES) {
            CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
            
            if (valueTypeId == boolenTypeId) {
                bool b = [value boolValue];
                
                variant = Mengine::ParamBool(b);
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
                        
                        variant = Mengine::ParamInteger(n);
                    }break;
                        
                    case kCFNumberFloat32Type:
                    case kCFNumberFloat64Type:
                    case kCFNumberFloatType:
                    case kCFNumberDoubleType: {
                        double d = [value doubleValue];
                        
                        variant = Mengine::ParamDouble(d);
                    }break;
                    
                    case kCFNumberCFIndexType:
                    case kCFNumberNSIntegerType:
                    case kCFNumberCGFloatType: {
                        
                    }break;
                }
            }
        } else if ([value isKindOfClass:[NSString class]] == YES) {
            Mengine::ConstString s = [AppleString NSStringToConstString:value];
            
            variant = Mengine::ParamConstString(s);
        } else if ([value isKindOfClass:[NSNull class]]) {
            variant = Mengine::ParamNull();
        } else {
            const Mengine::Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
            
            variant = Mengine::ParamString(value_str);
        }
        
        _out->emplace( key_cstr, variant );
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
                NSValue * ns_element = [NSValue valueWithPointer:_element.get()];
                
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
