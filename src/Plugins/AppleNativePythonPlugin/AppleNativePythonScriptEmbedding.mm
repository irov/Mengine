#include "AppleNativePythonScriptEmbedding.h"

#import "Environment/Apple/AppleIncluder.h"
#import "Environment/Apple/AppleSemaphoreListenerInterface.h"
#import "Environment/Apple/AppleDetail.h"
#import "Environment/Python/PythonScriptWrapper.h"
#import "Environment/Python/PythonCallbackProvider.h"

#import "AppleNativePythonPlugin.h"

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
                __block bool error = false;
                
                [AppleDetail visitParameters:_value forBool:^(NSString * key, BOOL value) {
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    pybind::dict_setobject_t( _kernel, py_dict, py_key, (bool)value );
                } forInteger:^(NSString * key, int64_t value) {
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    pybind::dict_setobject_t( _kernel, py_dict, py_key, value );
                } forDouble:^(NSString * key, double value) {
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    pybind::dict_setobject_t( _kernel, py_dict, py_key, value );
                } forString:^(NSString * key, NSString * value) {
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    pybind::dict_setobject_t( _kernel, py_dict, py_key, value );
                } forNull:^(NSString * key) {
                    PyObject * py_key = pybind::ptr( _kernel, key );
                    PyObject * py_none = _kernel->ret_none();
                    pybind::dict_setobject_t( _kernel, py_dict, py_key, py_none );
                } forUnknown:^(NSString * key, id value) {
                    error = true;
                }];
                
                if (error == true) {
                    return nullptr;
                }

                return py_dict;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_NSSet_type
            : public pybind::type_cast_result<NSSet *>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _nothrow );

                NSMutableSet * set = [NSMutableSet set];
                
                PyObject * it;
                if( _kernel->iterator_get( _obj, &it ) == false )
                {
                    return false;
                }
                
                PyObject * value;
                while( _kernel->iterator_next( it, &value ) )
                {
                    if( _kernel->bool_check( value ) == true )
                    {
                        bool value_bool = pybind::extract_t( _kernel, value );
                        
                        [set addObject:@(value_bool)];
                    }
                    else if ( _kernel->int_check( value ) == true )
                    {
                        int32_t value_int = pybind::extract_t( _kernel, value );
                        
                        [set addObject:@(value_int)];
                    }
                    else if ( _kernel->long_check( value ) == true )
                    {
                        int64_t value_long = pybind::extract_t( _kernel, value );
                        
                        [set addObject:@(value_long)];
                    }
                    else if ( _kernel->float_check( value ) == true )
                    {
                        float value_float = pybind::extract_t( _kernel, value );
                        
                        [set addObject:@(value_float)];
                    }
                    else if ( _kernel->string_check( value ) == true )
                    {
                        NSString * value_str = pybind::extract_t( _kernel, value );
                        
                        [set addObject:value_str];
                    }
                    else
                    {
                        return false;
                    }
                }
                
                _kernel->iterator_end( it );
                    
                _value = set;

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                PyObject * py_set = _kernel->set_new();
                __block bool error = false;
                
                [AppleDetail visitValues:_value forBool:^(BOOL value) {
                    pybind::set_set_t( _kernel, py_set, (bool)value );
                } forInteger:^(int64_t value) {
                    pybind::set_set_t( _kernel, py_set, value );
                } forDouble:^(double value) {
                    pybind::set_set_t( _kernel, py_set, value );
                } forString:^(NSString * value) {
                    pybind::set_set_t( _kernel, py_set, value );
                } forNull:^(void) {
                    PyObject * py_none = _kernel->ret_none();
                    pybind::set_set_t( _kernel, py_set, py_none );
                } forUnknown:^(id value) {
                    error = true;
                }];
                
                if (error == true) {
                    return nullptr;
                }

                return py_set;
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

                [AppleDetail addMainQueueOperation:^{
                    keep->call_cb();
                }];
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAppleSemaphoreListener, AppleSemaphoreListenerInterface> PythonAppleSemaphoreListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        void AppleNativePython_waitSemaphore( NSString * _name, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _name == nil )
            {
                _name = @"";
            }

            AppleSemaphoreListenerInterfacePtr listener = Helper::makeFactorableUnique<PythonAppleSemaphoreListener>( MENGINE_DOCUMENT_PYTHON, _cb, _args );

            [[AppleNativePythonPlugin sharedInstance] waitSemaphore:_name listener:listener];
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
        pybind::registration_type_cast<NSSet *>(_kernel, pybind::make_type_cast<Detail::extract_NSSet_type>(_kernel));
        
        pybind::def_function_args( _kernel, "waitSemaphore", &Detail::AppleNativePython_waitSemaphore );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_type_cast<NSString *>( _kernel );
        pybind::unregistration_type_cast<NSDictionary *>( _kernel );
        pybind::unregistration_type_cast<NSSet *>( _kernel );
    }
    //////////////////////////////////////////////////////////////////////////
}

