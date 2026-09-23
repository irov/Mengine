#include "iOSNativePythonTypeCast.h"

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    id extract_specialized<id>::operator () ( kernel_interface * _kernel, PyObject * _obj ) const
    {
        if( _kernel->is_none( _obj ) == true )
        {
            NSNull * value = [NSNull null];

            return value;
        }

        if( _kernel->bool_check( _obj ) == true )
        {
            bool value = pybind::extract_t( _kernel, _obj );

            NSNumber * number = @(value);

            return number;
        }

        if( _kernel->int_check( _obj ) == true || _kernel->long_check( _obj ) == true )
        {
            int64_t value = pybind::extract_t( _kernel, _obj );

            NSNumber * number = @(value);

            return number;
        }

        if( _kernel->float_check( _obj ) == true )
        {
            double value = pybind::extract_t( _kernel, _obj );

            NSNumber * number = @(value);

            return number;
        }

        if( _kernel->string_check( _obj ) == true )
        {
            NSString * value = pybind::extract_t( _kernel, _obj );

            return value;
        }

        if( _kernel->list_check( _obj ) == true || _kernel->tuple_check( _obj ) == true )
        {
            bool isList = _kernel->list_check( _obj );
            size_t count = isList == true ? _kernel->list_size( _obj ) : _kernel->tuple_size( _obj );
            NSMutableArray * result = [NSMutableArray arrayWithCapacity:count];

            for( size_t index = 0; index != count; ++index )
            {
                PyObject * item = isList == true ? _kernel->list_getitem( _obj, index ) : _kernel->tuple_getitem( _obj, index );
                id value = pybind::extract_t( _kernel, item );

                [result addObject:value];
            }

            return result;
        }

        if( _kernel->dict_check( _obj ) == true )
        {
            NSMutableDictionary * result = [NSMutableDictionary dictionary];
            size_t position = 0;

            PyObject * key;
            PyObject * value;
            while( _kernel->dict_next( _obj, &position, &key, &value ) == true )
            {
                NSString * nativeKey = pybind::extract_t( _kernel, key );
                id nativeValue = pybind::extract_t( _kernel, value );

                [result setObject:nativeValue forKey:nativeKey];
            }

            return result;
        }

        pybind::throw_exception( "unsupported Python value for Objective-C conversion" );

        return nil;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * ptr_throw_specialized<id>::operator () ( kernel_interface * _kernel, const id & _value ) const
    {
        if( _value == nil || [_value isKindOfClass:[NSNull class]] == YES )
        {
            PyObject * py_none = _kernel->ret_none();

            return py_none;
        }

        if( [_value isKindOfClass:[NSString class]] == YES )
        {
            NSString * value = (NSString *)_value;
            PyObject * py_value = pybind::ptr_throw( _kernel, value );

            return py_value;
        }

        if( [_value isKindOfClass:[NSNumber class]] == YES )
        {
            if( CFGetTypeID( (__bridge CFTypeRef)_value ) == CFBooleanGetTypeID() )
            {
                bool value = [(NSNumber *)_value boolValue] == YES;
                PyObject * py_value = _kernel->ret_bool( value );

                return py_value;
            }

            if( CFNumberIsFloatType( (__bridge CFNumberRef)_value ) == true )
            {
                double value = [(NSNumber *)_value doubleValue];
                PyObject * py_value = _kernel->ptr_double( value );

                return py_value;
            }

            int64_t value = [(NSNumber *)_value longLongValue];
            PyObject * py_value = _kernel->ptr_int64( value );

            return py_value;
        }

        if( [_value isKindOfClass:[NSArray class]] == YES )
        {
            pybind::list result( _kernel );

            for( id item in (NSArray *)_value )
            {
                result.append( item );
            }

            PyObject * py_value = result.ret();

            return py_value;
        }

        if( [_value isKindOfClass:[NSDictionary class]] == YES )
        {
            pybind::dict result( _kernel );

            for( id key in (NSDictionary *)_value )
            {
                id value = [(NSDictionary *)_value objectForKey:key];

                result[key] = value;
            }

            PyObject * py_value = result.ret();

            return py_value;
        }

        if( [_value isKindOfClass:[NSSet class]] == YES )
        {
            pybind::set result( _kernel );

            for( id item in (NSSet *)_value )
            {
                result.add( item );
            }

            PyObject * py_value = result.ret();

            return py_value;
        }

        Class value_class = [_value class];
        NSString * value_class_name = NSStringFromClass( value_class );
        const char * value_class_name_str = [value_class_name UTF8String];

        pybind::throw_exception( "unsupported Objective-C value class '%s'", value_class_name_str );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
