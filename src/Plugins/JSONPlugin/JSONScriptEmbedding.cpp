#include "JSONScriptEmbedding.h"

#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "ResourceJSON.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/StdIO.h"

#include <cmath>
#include <cstring>
#include <limits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        struct json_encode_context_t
        {
            pybind::kernel_interface * kernel;
            Vector<PyObject *> ancestors;
        };
        //////////////////////////////////////////////////////////////////////////
        struct json_decode_error_t
        {
            int32_t line;
            int32_t column;
            int32_t position;
            jpp::load_error_code_e code;
            String message;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool __json_enter_container( json_encode_context_t * const _context, PyObject * _value )
        {
            for( PyObject * ancestor : _context->ancestors )
            {
                if( ancestor == _value )
                {
                    _context->kernel->set_error( pybind::error_type_e::Value, "circular reference detected" );

                    return false;
                }
            }

            _context->ancestors.emplace_back( _value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void __json_leave_container( json_encode_context_t * const _context )
        {
            _context->ancestors.pop_back();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool __json_get_utf8( pybind::kernel_interface * _kernel, PyObject * _value, pybind::object * const _utf8Owner, const char ** const _utf8, size_t * const _size )
        {
            if( _kernel->string_check( _value ) == true )
            {
                *_utf8 = _kernel->string_to_char_and_size( _value, _size );

                return true;
            }

            if( _kernel->unicode_check( _value ) == true )
            {
                PyObject * py_utf8 = _kernel->unicode_encode_utf8( _value );

                if( py_utf8 == nullptr )
                {
                    return false;
                }

                *_utf8Owner = pybind::object( _kernel, py_utf8, pybind::borrowed );
                *_utf8 = _kernel->string_to_char_and_size( py_utf8, _size );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool __pybind_to_jpp( json_encode_context_t * const _context, PyObject * _value, jpp::object * const _result )
        {
            pybind::kernel_interface * kernel = _context->kernel;

            if( kernel->is_none( _value ) == true )
            {
                *_result = jpp::make_null();

                return true;
            }

            if( kernel->bool_check( _value ) == true )
            {
                *_result = jpp::make_boolean( kernel->is_true( _value ) );

                return true;
            }

            if( kernel->int_check( _value ) == true || kernel->long_check( _value ) == true )
            {
                int64_t integer;

                if( kernel->extract_int64( _value, integer ) == false )
                {
                    kernel->set_error( pybind::error_type_e::Overflow, "integer value is outside signed int64 range" );

                    return false;
                }

                *_result = jpp::make_integer( integer );

                return true;
            }

            if( kernel->float_check( _value ) == true )
            {
                double real;

                if( kernel->extract_double( _value, real ) == false )
                {
                    kernel->set_error( pybind::error_type_e::Type, "unable to extract floating-point value" );

                    return false;
                }

                if( std::isfinite( real ) == false )
                {
                    kernel->set_error( pybind::error_type_e::Value, "NaN and Infinity are not valid JSON numbers" );

                    return false;
                }

                *_result = jpp::make_real( real );

                return true;
            }

            if( kernel->string_check( _value ) == true || kernel->unicode_check( _value ) == true )
            {
                pybind::object utf8Owner;
                const char * utf8;
                size_t size;

                if( __json_get_utf8( kernel, _value, &utf8Owner, &utf8, &size ) == false )
                {
                    kernel->set_error( pybind::error_type_e::Value, "string value is not valid UTF-8" );

                    return false;
                }

                jpp::object string = jpp::make_stringn( utf8, size );

                if( string.invalid() == true )
                {
                    kernel->set_error( pybind::error_type_e::Value, "string value is not valid UTF-8" );

                    return false;
                }

                *_result = string;

                return true;
            }

            if( kernel->list_check( _value ) == true || kernel->tuple_check( _value ) == true )
            {
                if( __json_enter_container( _context, _value ) == false )
                {
                    return false;
                }

                jpp::array array = jpp::make_array();
                size_t count = kernel->list_check( _value ) == true
                    ? kernel->list_size( _value )
                    : kernel->tuple_size( _value );

                for( size_t index = 0; index != count; ++index )
                {
                    PyObject * py_item = kernel->list_check( _value ) == true
                        ? kernel->list_getitem( _value, index )
                        : kernel->tuple_getitem( _value, index );

                    jpp::object item;

                    if( __pybind_to_jpp( _context, py_item, &item ) == false )
                    {
                        __json_leave_container( _context );

                        return false;
                    }

                    array.push_back( item );
                }

                __json_leave_container( _context );

                *_result = array.to_object();

                return true;
            }

            if( kernel->dict_check( _value ) == true )
            {
                if( __json_enter_container( _context, _value ) == false )
                {
                    return false;
                }

                jpp::object object = jpp::make_object();
                size_t position = 0;
                PyObject * py_key;
                PyObject * py_item;

                while( kernel->dict_next( _value, &position, &py_key, &py_item ) == true )
                {
                    if( kernel->string_check( py_key ) == false && kernel->unicode_check( py_key ) == false )
                    {
                        kernel->set_error( pybind::error_type_e::Type, "JSON object keys must be str or unicode" );
                        __json_leave_container( _context );

                        return false;
                    }

                    pybind::object utf8KeyOwner;
                    const char * utf8Key;
                    size_t utf8KeySize;

                    if( __json_get_utf8( kernel, py_key, &utf8KeyOwner, &utf8Key, &utf8KeySize ) == false )
                    {
                        kernel->set_error( pybind::error_type_e::Value, "JSON object key is not valid UTF-8" );
                        __json_leave_container( _context );

                        return false;
                    }

                    jpp::object item;

                    if( __pybind_to_jpp( _context, py_item, &item ) == false )
                    {
                        __json_leave_container( _context );

                        return false;
                    }

                    if( object.setn( utf8Key, utf8KeySize, item ) == false )
                    {
                        kernel->set_error( pybind::error_type_e::Value, "JSON object key is not valid UTF-8" );
                        __json_leave_container( _context );

                        return false;
                    }
                }

                __json_leave_container( _context );

                *_result = object;

                return true;
            }

            kernel->set_error( pybind::error_type_e::Type, "value type is not supported by encodeJSON" );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * __jpp_to_pybind( pybind::kernel_interface * _kernel, const jpp::object & _object )
        {
            jpp::e_type type = _object.type();

            switch( type )
            {
            case jpp::e_type::JPP_OBJECT:
                {
                    PyObject * py_dict = _kernel->dict_new_presized( _object.size() );

                    if( py_dict == nullptr )
                    {
                        return nullptr;
                    }

                    for( jpp::object_iterator iterator = jpp::begin( _object ), iteratorEnd = jpp::end( _object ); iterator != iteratorEnd; ++iterator )
                    {
                        jpp::object_iterator_pair pair = *iterator;
                        PyObject * py_key = _kernel->unicode_from_utf8_size( pair.key, iterator.key_size() );

                        if( py_key == nullptr )
                        {
                            _kernel->decref( py_dict );

                            return nullptr;
                        }

                        PyObject * py_value = __jpp_to_pybind( _kernel, pair.value );

                        if( py_value == nullptr )
                        {
                            _kernel->decref( py_key );
                            _kernel->decref( py_dict );

                            return nullptr;
                        }

                        bool successful = _kernel->dict_set( py_dict, py_key, py_value );
                        _kernel->decref( py_key );
                        _kernel->decref( py_value );

                        if( successful == false )
                        {
                            _kernel->decref( py_dict );

                            return nullptr;
                        }
                    }

                    return py_dict;
                }break;
            case jpp::e_type::JPP_ARRAY:
                {
                    jpp::array array( _object );
                    PyObject * py_list = _kernel->list_new( array.size() );

                    if( py_list == nullptr )
                    {
                        return nullptr;
                    }

                    for( size_t index = 0; index != array.size(); ++index )
                    {
                        PyObject * py_value = __jpp_to_pybind( _kernel, array[index] );

                        if( py_value == nullptr )
                        {
                            _kernel->decref( py_list );

                            return nullptr;
                        }

                        bool successful = _kernel->list_setitem( py_list, index, py_value );
                        _kernel->decref( py_value );

                        if( successful == false )
                        {
                            _kernel->decref( py_list );

                            return nullptr;
                        }
                    }

                    return py_list;
                }break;
            case jpp::e_type::JPP_STRING:
                {
                    jpp::jpp_size_t size;
                    const char * value = jpp::get_string_and_size( _object, &size );

                    return _kernel->unicode_from_utf8_size( value, size );
                }break;
            case jpp::e_type::JPP_INTEGER:
                {
                    int64_t value = _object;

                    return _kernel->ptr_integer( value );
                }break;
            case jpp::e_type::JPP_REAL:
                {
                    double value = _object;

                    return _kernel->ptr_double( value );
                }break;
            case jpp::e_type::JPP_TRUE:
                {
                    return _kernel->ret_true();
                }break;
            case jpp::e_type::JPP_FALSE:
                {
                    return _kernel->ret_false();
                }break;
            case jpp::e_type::JPP_NULL:
                {
                    return _kernel->ret_none();
                }break;
            }

            _kernel->set_error( pybind::error_type_e::Runtime, "invalid native JSON value type" );

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static int __json_dump_string( const char * _buffer, jpp::jpp_size_t _size, void * _userData )
        {
            String * output = static_cast<String *>(_userData);

            output->append( _buffer, _size );

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static void __json_decode_error( int32_t _line, int32_t _column, int32_t _position, const char * _source, const char * _text, jpp::load_error_code_e _code, void * _userData )
        {
            MENGINE_UNUSED( _source );

            json_decode_error_t * error = static_cast<json_decode_error_t *>(_userData);

            error->line = _line;
            error->column = _column;
            error->position = _position;
            error->code = _code;
            error->message.assign( _text );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool __json_parse_indent( pybind::kernel_interface * _kernel, PyObject * _value, bool * const _compact, size_t * const _indent )
        {
            if( _kernel->is_none( _value ) == true )
            {
                *_compact = true;
                *_indent = 0;

                return true;
            }

            if( _kernel->bool_check( _value ) == true || (_kernel->int_check( _value ) == false && _kernel->long_check( _value ) == false) )
            {
                _kernel->set_error( pybind::error_type_e::Type, "indent must be None or an integer" );

                return false;
            }

            int64_t indent;

            if( _kernel->extract_int64( _value, indent ) == false )
            {
                _kernel->set_error( pybind::error_type_e::Overflow, "indent is outside signed int64 range" );

                return false;
            }

            if( indent < 0 )
            {
                _kernel->set_error( pybind::error_type_e::Value, "indent must be greater than or equal to zero" );

                return false;
            }

            if( static_cast<uint64_t>(indent) > static_cast<uint64_t>(std::numeric_limits<size_t>::max()) )
            {
                _kernel->set_error( pybind::error_type_e::Overflow, "indent is outside platform size range" );

                return false;
            }

            *_compact = false;
            *_indent = static_cast<size_t>(indent);

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * __encode_json( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
        {
            size_t argumentCount = _kernel->tuple_size( _args );

            if( argumentCount < 1 || argumentCount > 2 )
            {
                _kernel->set_error( pybind::error_type_e::Type, "encodeJSON expects one or two positional arguments" );

                return nullptr;
            }

            PyObject * indentValue = argumentCount == 2
                ? _kernel->tuple_getitem( _args, 1 )
                : _kernel->get_none();
            bool hasPositionalIndent = argumentCount == 2;

            if( _kwds != nullptr )
            {
                size_t position = 0;
                PyObject * py_key;
                PyObject * py_value;

                while( _kernel->dict_next( _kwds, &position, &py_key, &py_value ) == true )
                {
                    if( _kernel->string_check( py_key ) == false )
                    {
                        _kernel->set_error( pybind::error_type_e::Type, "encodeJSON keyword names must be strings" );

                        return nullptr;
                    }

                    size_t keySize;
                    const char * key = _kernel->string_to_char_and_size( py_key, &keySize );

                    if( keySize != 6 || std::memcmp( key, "indent", 6 ) != 0 )
                    {
                        _kernel->set_error( pybind::error_type_e::Type, "encodeJSON received an unknown keyword argument" );

                        return nullptr;
                    }

                    if( hasPositionalIndent == true )
                    {
                        _kernel->set_error( pybind::error_type_e::Type, "encodeJSON received indent more than once" );

                        return nullptr;
                    }

                    indentValue = py_value;
                    hasPositionalIndent = true;
                }
            }

            bool compact;
            size_t indent;

            if( __json_parse_indent( _kernel, indentValue, &compact, &indent ) == false )
            {
                return nullptr;
            }

            json_encode_context_t context = {_kernel, {}};
            jpp::object json;
            PyObject * py_value = _kernel->tuple_getitem( _args, 0 );

            if( __pybind_to_jpp( &context, py_value, &json ) == false )
            {
                return nullptr;
            }

            String output;
            bool successful = compact == true
                ? jpp::dump_compact( json, &__json_dump_string, &output )
                : jpp::dump( json, indent, &__json_dump_string, &output );

            if( successful == false )
            {
                _kernel->set_error( pybind::error_type_e::Runtime, "unable to encode native JSON value" );

                return nullptr;
            }

            return _kernel->string_from_char_size( output.data(), output.size() );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * __decode_json( pybind::kernel_interface * _kernel, PyObject * _args, PyObject * _kwds )
        {
            if( _kernel->tuple_size( _args ) != 1 || (_kwds != nullptr && _kernel->dict_size( _kwds ) != 0) )
            {
                _kernel->set_error( pybind::error_type_e::Type, "decodeJSON expects exactly one positional argument" );

                return nullptr;
            }

            PyObject * py_text = _kernel->tuple_getitem( _args, 0 );

            if( _kernel->string_check( py_text ) == false && _kernel->unicode_check( py_text ) == false )
            {
                _kernel->set_error( pybind::error_type_e::Type, "decodeJSON expects str or unicode" );

                return nullptr;
            }

            pybind::object utf8Owner;
            const char * utf8;
            size_t size;

            if( __json_get_utf8( _kernel, py_text, &utf8Owner, &utf8, &size ) == false )
            {
                _kernel->set_error( pybind::error_type_e::Value, "decodeJSON input is not valid UTF-8" );

                return nullptr;
            }

            json_decode_error_t parseError = {0, 0, 0, jpp::load_error_code_e::invalid, {}};
            jpp::object json = jpp::load( utf8, size, jpp::JPP_LOAD_MODE_DECODE_ANY | jpp::JPP_LOAD_MODE_ALLOW_NUL, &__json_decode_error, &parseError );

            if( json.invalid() == true )
            {
                Char message[1024];
                MENGINE_SNPRINTF( message, 1024, "JSON decode error at line %d column %d: %s"
                    , parseError.line
                    , parseError.column
                    , parseError.message.c_str()
                );

                pybind::error_type_e errorType = parseError.code == jpp::load_error_code_e::numeric_overflow
                    ? pybind::error_type_e::Overflow
                    : pybind::error_type_e::Value;

                _kernel->set_error( errorType, message );

                return nullptr;
            }

            PyObject * result = __jpp_to_pybind( _kernel, json );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object __get_json( pybind::kernel_interface * _kernel, const ResourceJSON * _resource )
        {
            const jpp::object & json = _resource->getJSON();
            PyObject * py_json = __jpp_to_pybind( _kernel, json );

            if( py_json == nullptr )
            {
                return pybind::make_invalid_object_t();
            }

            return pybind::object( _kernel, py_json, pybind::borrowed );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    JSONScriptEmbedding::JSONScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONScriptEmbedding::~JSONScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function_native( _kernel, "encodeJSON", &Helper::__encode_json );
        pybind::def_function_native( _kernel, "decodeJSON", &Helper::__decode_json );

        pybind::interface_<ResourceJSON, pybind::bases<Resource>>( _kernel, "ResourceJSON", false )
            .def_static_kernel( "getJSON", &Helper::__get_json )
            ;

        Helper::registerScriptWrapping<ResourceJSON>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "encodeJSON", nullptr );
        _kernel->remove_from_module( "decodeJSON", nullptr );

        _kernel->remove_scope<ResourceJSON>();

        Helper::unregisterScriptWrapping<ResourceJSON>();
    }
    //////////////////////////////////////////////////////////////////////////
}
