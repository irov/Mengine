#include "AppleNativePythonScriptEmbedding.h"

#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/Python/PythonScriptWrapper.h"
#import "Environment/Python/PythonCallbackProvider.h"

#include "AppleNativePythonInterface.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSString_type
            : public pybind::type_cast_result<NSString *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                if( _kernel->string_check( _obj ) == true )
                {
                    const Char * value_char = _kernel->string_to_char( _obj );

                    if( value_char == nullptr )
                    {
                        return false;
                    }

                    _value = [NSString stringWithUTF8String:value_char];
                }
                else
                {
                    return false;
                }

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                const Char * value_str = [_value UTF8String];

                PyObject * py_value = _kernel->string_from_char( value_str );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSDictionary_type
            : public pybind::type_cast_result<NSDictionary *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                if( _kernel->dict_check( _obj ) == true )
                {
                    NSMutableDictionary * dict = [NSMutableDictionary dictionary];

                    size_t pos;
                    PyObject * key;
                    PyObject * value;

                    while( _kernel->dict_next( _obj, &pos, &key, &value ) == true )
                    {
                        NSString * key_str = pybind::extract_t( _kernel, key );
                        
                        PyTypeObject * type = _kernel->get_object_type( value );
                        
                        if( _kernel->bool_check( value ) == true )
                        {
                            bool value_bool = pybind::extract_t( _kernel, value );
                            
                            [dict setObject:@(value_bool) forKey:key_str];
                        }
                        else if ( _kernel->int_check( value ) == true )
                        {
                            int32_t value_int = pybind::extract_t( _kernel, value );
                            
                            [dict setObject:@(value_int) forKey:key_str];
                        }
                        else if ( _kernel->long_check( value ) == true )
                        {
                            int64_t value_long = pybind::extract_t( _kernel, value );
                            
                            [dict setObject:@(value_long) forKey:key_str];
                        }
                        else if ( _kernel->float_check( value ) == true )
                        {
                            float value_float = pybind::extract_t( _kernel, value );
                            
                            [dict setObject:@(value_float) forKey:key_str];
                        }
                        else if ( _kernel->string_check( value ) == true )
                        {
                            NSString * value_str = pybind::extract_t( _kernel, value );
                            
                            [dict setObject:value_str forKey:key_str];
                        }
                        else
                        {
                            return false;
                        }
                    }
                    
                    _value = dict;
                }
                else
                {
                    return false;
                }

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                PyObject * py_dict = _kernel->dict_new();
                
                CFTypeID boolenTypeId = CFBooleanGetTypeID();
                CFTypeID numberTypeId = CFNumberGetTypeID();
                
                for (NSString * key in _value) {
                    id value = [_value objectForKey:key];
                    
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    
                    if ([value isKindOfClass:[NSNumber class]] == YES) {
                        CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
                        
                        if (valueTypeId == boolenTypeId) {
                            bool b = [value boolValue];
                            
                            pybind::dict_setobject_t( _kernel, py_dict, py_key, b );
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
                                    
                                    pybind::dict_setobject_t( _kernel, py_dict, py_key, n );
                                }break;
                                    
                                case kCFNumberFloat32Type:
                                case kCFNumberFloat64Type:
                                case kCFNumberFloatType:
                                case kCFNumberDoubleType: {
                                    double d = [value doubleValue];
                                    
                                    pybind::dict_setobject_t( _kernel, py_dict, py_key, d );
                                }break;
                                
                                case kCFNumberCFIndexType:
                                case kCFNumberNSIntegerType:
                                case kCFNumberCGFloatType: {
                                    
                                }break;
                            }
                        }
                    } else if ([value isKindOfClass:[NSString class]] == YES) {
                        NSString * s = (NSString *)value;
                        
                        pybind::dict_setobject_t( _kernel, py_dict, py_key, s );
                    } else if ([value isKindOfClass:[NSNull class]]) {
                        //Empty (???)
                    } else {
                        return nullptr;
                    }
                }

                return py_dict;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        class PythonAppleSemaphoreListener
            : public AppleSemaphoreListenerInterface
            , public PythonCallbackProvider
        {
            DECLARE_FACTORABLE("PythonAppleSemaphoreListener")
            
        public:
            PythonAppleSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
                : PythonCallbackProvider( _cb, _args )
            {
            }

        protected:
            void invoke() override
            {
                PythonCallbackProviderPtr keep = PythonCallbackProviderPtr::from(this);

                [AppleDetail dispatchMainQueue:^{
                    keep->call_cb();
                }];
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleSemaphoreListener, AppleSemaphoreListenerInterface> PythonAppleSemaphoreListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        void AppleNativePythonService_waitSemaphore( const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            AppleSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAppleSemaphoreListener>( MENGINE_DOCUMENT_PYBIND, _cb, _args );

            APPLE_NATIVEPYTHON_SERVICE()
                ->waitSemaphore( _name, listener );
        }
        ///////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonScriptEmbedding::AppleNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonScriptEmbedding::~AppleNativePythonScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleNativePythonScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::registration_type_cast<NSString *>(_kernel, pybind::make_type_cast<Detail::extract_NSString_type>(_kernel));
        pybind::registration_type_cast<NSDictionary *>(_kernel, pybind::make_type_cast<Detail::extract_NSDictionary_type>(_kernel));
        
        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::AppleNativePythonService_waitSemaphore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}

