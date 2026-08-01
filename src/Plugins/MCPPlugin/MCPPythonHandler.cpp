#include "MCPPythonHandler.h"

#include "pybind/dict.hpp"
#include "pybind/list.hpp"
#include "pybind/tuple.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool mcpJSONToScript( pybind::kernel_interface * _kernel, const jpp::object & _value, pybind::object * const _result )
        {
            switch( _value.type() )
            {
            case jpp::e_type::JPP_NULL:
                {
                    pybind::object value = pybind::make_none_t( _kernel );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_TRUE:
                {
                    pybind::object value = pybind::make_true_t( _kernel );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_FALSE:
                {
                    pybind::object value = pybind::make_false_t( _kernel );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_INTEGER:
                {
                    pybind::object value = pybind::make_object_t( _kernel, (int64_t)_value );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_REAL:
                {
                    pybind::object value = pybind::make_object_t( _kernel, (double)_value );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_STRING:
                {
                    pybind::object value = pybind::make_object_t( _kernel, (const Char *)_value );

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_ARRAY:
                {
                    pybind::list value( _kernel );

                    for( const jpp::object & item : jpp::array( _value ) )
                    {
                        pybind::object scriptItem;
                        bool converted = Detail::mcpJSONToScript( _kernel, item, &scriptItem );

                        if( converted == false )
                        {
                            return false;
                        }

                        value.append( scriptItem );
                    }

                    *_result = value;

                    return true;
                }break;
            case jpp::e_type::JPP_OBJECT:
                {
                    pybind::dict value( _kernel );

                    for( const jpp::object_iterator_pair & item : _value )
                    {
                        pybind::object scriptValue;
                        bool converted = Detail::mcpJSONToScript( _kernel, item.value, &scriptValue );

                        if( converted == false )
                        {
                            return false;
                        }

                        value.set( item.key, scriptValue );
                    }

                    *_result = value;

                    return true;
                }break;
            default:
                break;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool mcpScriptToJSON( pybind::kernel_interface * _kernel, PyObject * _value, uint32_t _depth, jpp::object * const _result )
        {
            if( _depth > 16 )
            {
                return false;
            }

            if( _kernel->is_none( _value ) == true )
            {
                jpp::object value = jpp::make_null();

                *_result = value;

                return true;
            }

            if( _kernel->bool_check( _value ) == true )
            {
                bool value = _kernel->is_true( _value );
                jpp::object jsonValue = jpp::make_boolean( value );

                *_result = jsonValue;

                return true;
            }

            if( _kernel->int_check( _value ) == true || _kernel->long_check( _value ) == true )
            {
                int64_t value = 0;

                if( _kernel->extract_int64( _value, value ) == false )
                {
                    return false;
                }

                jpp::object jsonValue = jpp::make_integer( value );

                *_result = jsonValue;

                return true;
            }

            if( _kernel->float_check( _value ) == true )
            {
                double value = 0.0;

                if( _kernel->extract_double( _value, value ) == false )
                {
                    return false;
                }

                jpp::object jsonValue = jpp::make_real( value );

                *_result = jsonValue;

                return true;
            }

            if( _kernel->string_check( _value ) == true )
            {
                const Char * value = _kernel->string_to_char( _value );
                jpp::object jsonValue = jpp::make_string( value );

                *_result = jsonValue;

                return true;
            }

            if( _kernel->list_check( _value ) == true )
            {
                size_t size = _kernel->list_size( _value );
                jpp::array jsonArray = jpp::make_array();

                for( size_t index = 0; index != size; ++index )
                {
                    PyObject * item = _kernel->list_getitem( _value, index );
                    jpp::object jsonItem;
                    bool converted = Detail::mcpScriptToJSON( _kernel, item, _depth + 1, &jsonItem );

                    if( converted == false )
                    {
                        return false;
                    }

                    jsonArray.push_back( jsonItem );
                }

                jpp::object value = jsonArray.to_object();

                *_result = value;

                return true;
            }

            if( _kernel->tuple_check( _value ) == true )
            {
                size_t size = _kernel->tuple_size( _value );
                jpp::array jsonArray = jpp::make_array();

                for( size_t index = 0; index != size; ++index )
                {
                    PyObject * item = _kernel->tuple_getitem( _value, index );
                    jpp::object jsonItem;
                    bool converted = Detail::mcpScriptToJSON( _kernel, item, _depth + 1, &jsonItem );

                    if( converted == false )
                    {
                        return false;
                    }

                    jsonArray.push_back( jsonItem );
                }

                jpp::object value = jsonArray.to_object();

                *_result = value;

                return true;
            }

            if( _kernel->dict_check( _value ) == true )
            {
                jpp::object jsonObject = jpp::make_object();
                size_t position = 0;
                PyObject * key = nullptr;
                PyObject * value = nullptr;

                while( _kernel->dict_next( _value, &position, &key, &value ) == true )
                {
                    if( _kernel->string_check( key ) == false )
                    {
                        return false;
                    }

                    const Char * jsonKey = _kernel->string_to_char( key );
                    jpp::object jsonValue;
                    bool converted = Detail::mcpScriptToJSON( _kernel, value, _depth + 1, &jsonValue );

                    if( converted == false )
                    {
                        return false;
                    }

                    jsonObject.set( jsonKey, jsonValue );
                }

                *_result = jsonObject;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPPythonHandler::MCPPythonHandler( const pybind::object & _callback, const pybind::args & _args )
        : PythonCallbackProvider( _callback, _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPPythonHandler::~MCPPythonHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPythonHandler::onMCPCall( const jpp::object & _arguments, jpp::object * const _result )
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::object arguments;
        bool converted = Detail::mcpJSONToScript( kernel, _arguments, &arguments );

        if( converted == false )
        {
            return false;
        }

        pybind::object response = this->call_cb( arguments );
        PyObject * responseObject = response.ptr();
        jpp::object result;
        bool serialized = Detail::mcpScriptToJSON( kernel, responseObject, 0, &result );

        if( serialized == false )
        {
            return false;
        }

        *_result = result;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
