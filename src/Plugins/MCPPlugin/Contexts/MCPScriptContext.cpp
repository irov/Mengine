#include "MCPScriptContext.h"

#include "Config/StdUtility.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptProviderServiceInterface.h"

#   include "pybind/dict.hpp"
#   include "pybind/list.hpp"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptContext::MCPScriptContext()
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        : m_nextHandle( 1 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPScriptContext::finalize()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        m_handles.clear();
        m_sources.clear();
        m_globals = nullptr;
        m_nextHandle = 1;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPScriptContext::getHandleCount() const
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        size_t count = m_handles.size();

        return count;
#else
        return 0;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    bool MCPScriptContext::jsonToScript( const jpp::object & _value, pybind::object * const _result, String * const _error ) const
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();

        switch( _value.type() )
        {
        case jpp::e_type::JPP_NULL:
            {
                pybind::object value = pybind::make_none_t( kernel );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_TRUE:
            {
                pybind::object value = pybind::make_true_t( kernel );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_FALSE:
            {
                pybind::object value = pybind::make_false_t( kernel );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_INTEGER:
            {
                int64_t integer = _value;
                pybind::object value = pybind::make_object_t( kernel, integer );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_REAL:
            {
                double real = _value;
                pybind::object value = pybind::make_object_t( kernel, real );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_STRING:
            {
                jpp::jpp_size_t size = 0;
                const Char * stringValue = jpp::get_string_and_size( _value, &size );
                PyObject * stringObject = kernel->string_from_char_size( stringValue, size );
                pybind::object value( kernel, stringObject );
                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_ARRAY:
            {
                pybind::list value( kernel );
                jpp::array array( _value );

                for( const jpp::object & item : array )
                {
                    pybind::object converted;
                    bool successful = this->jsonToScript( item, &converted, _error );

                    if( successful == false )
                    {
                        return false;
                    }

                    value.append( converted );
                }

                *_result = value;

                return true;
            }break;
        case jpp::e_type::JPP_OBJECT:
            {
                pybind::dict value( kernel );

                for( const jpp::object_iterator_pair & item : _value )
                {
                    pybind::object converted;
                    bool successful = this->jsonToScript( item.value, &converted, _error );

                    if( successful == false )
                    {
                        return false;
                    }

                    value.set( item.key, converted );
                }

                *_result = value;

                return true;
            }break;
        default:
            {
            }break;
        }

        *_error = "unsupported JSON value";

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptContext::scriptToJSON( const pybind::object & _value, uint32_t _maxDepth, uint32_t _maxItems, uint32_t _start, jpp::object * const _result, String * const _error )
    {
        Set<PyObject *> visited;
        bool successful = this->scriptToJSON_( _value, 0, _maxDepth, _maxItems, _start, &visited, _result, _error );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptContext::scriptToJSON_( const pybind::object & _value, uint32_t _depth, uint32_t _maxDepth, uint32_t _maxItems, uint32_t _start, Set<PyObject *> * const _visited, jpp::object * const _result, String * const _error )
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        PyObject * object = _value.ptr();

        if( _value.is_invalid() == true )
        {
            *_error = "Python operation returned an invalid object";

            return false;
        }

        if( _value.is_none() == true )
        {
            jpp::object value = jpp::make_null();
            *_result = value;

            return true;
        }

        if( _value.is_bool() == true )
        {
            bool extracted = false;
            kernel->extract_bool( object, extracted );
            jpp::object value = jpp::make_boolean( extracted );
            *_result = value;

            return true;
        }

        if( _value.is_integer() == true || _value.is_long() == true )
        {
            int64_t extracted = 0;

            if( kernel->extract_int64( object, extracted ) == true )
            {
                jpp::object value = jpp::make_integer( extracted );
                *_result = value;

                return true;
            }
        }

        if( _value.is_float() == true )
        {
            double extracted = 0.0;

            if( kernel->extract_double( object, extracted ) == true )
            {
                jpp::object value = jpp::make_real( extracted );
                *_result = value;

                return true;
            }
        }

        if( _value.is_string() == true )
        {
            size_t size = 0;
            const Char * stringValue = kernel->string_to_char_and_size( object, &size );
            jpp::object value = jpp::make_stringn( stringValue, size );
            *_result = value;

            return true;
        }

        if( _value.is_unicode() == true )
        {
            PyObject * encodedObject = kernel->unicode_encode_utf8( object );
            pybind::object encoded( kernel, encodedObject );
            size_t size = 0;
            PyObject * encodedValue = encoded.ptr();
            const Char * stringValue = kernel->string_to_char_and_size( encodedValue, &size );
            jpp::object value = jpp::make_stringn( stringValue, size );
            *_result = value;

            return true;
        }

        if( _depth < _maxDepth && _value.is_list() == true )
        {
            if( _visited->emplace( object ).second == false )
            {
                jpp::object cycle = jpp::make_object();
                cycle.set( "cycle", true );
                *_result = cycle;

                return true;
            }

            size_t size = kernel->list_size( object );
            size_t begin = _depth == 0 && _start < size ? _start : (_depth == 0 ? size : 0);
            size_t remaining = size - begin;
            size_t count = remaining < _maxItems ? remaining : _maxItems;
            jpp::array array = jpp::make_array();

            for( size_t index = begin; index != begin + count; ++index )
            {
                PyObject * item = kernel->list_getitem( object, index );
                pybind::object itemObject( kernel, item );
                jpp::object converted;
                bool successful = this->scriptToJSON_( itemObject, _depth + 1, _maxDepth, _maxItems, 0, _visited, &converted, _error );

                if( successful == false )
                {
                    return false;
                }

                array.push_back( converted );
            }

            _visited->erase( object );
            jpp::object value = array.to_object();
            *_result = value;

            return true;
        }

        if( _depth < _maxDepth && kernel->tuple_check( object ) == true )
        {
            if( _visited->emplace( object ).second == false )
            {
                jpp::object cycle = jpp::make_object();
                cycle.set( "cycle", true );
                *_result = cycle;

                return true;
            }

            size_t size = kernel->tuple_size( object );
            size_t begin = _depth == 0 && _start < size ? _start : (_depth == 0 ? size : 0);
            size_t remaining = size - begin;
            size_t count = remaining < _maxItems ? remaining : _maxItems;
            jpp::array array = jpp::make_array();

            for( size_t index = begin; index != begin + count; ++index )
            {
                PyObject * item = kernel->tuple_getitem( object, index );
                pybind::object itemObject( kernel, item );
                jpp::object converted;
                bool successful = this->scriptToJSON_( itemObject, _depth + 1, _maxDepth, _maxItems, 0, _visited, &converted, _error );

                if( successful == false )
                {
                    return false;
                }

                array.push_back( converted );
            }

            _visited->erase( object );
            jpp::object value = array.to_object();
            *_result = value;

            return true;
        }

        if( _depth < _maxDepth && _value.is_dict() == true )
        {
            if( _visited->emplace( object ).second == false )
            {
                jpp::object cycle = jpp::make_object();
                cycle.set( "cycle", true );
                *_result = cycle;

                return true;
            }

            jpp::object valueObject = jpp::make_object();
            size_t position = 0;
            size_t skipped = 0;
            size_t start = _depth == 0 ? _start : 0;
            size_t count = 0;
            PyObject * key = nullptr;
            PyObject * value = nullptr;

            while( count != _maxItems && kernel->dict_next( object, &position, &key, &value ) == true )
            {
                if( kernel->string_check( key ) == false )
                {
                    break;
                }

                if( skipped != start )
                {
                    ++skipped;

                    continue;
                }

                const Char * keyString = kernel->string_to_char( key );
                pybind::object scriptValue( kernel, value );
                jpp::object converted;
                bool successful = this->scriptToJSON_( scriptValue, _depth + 1, _maxDepth, _maxItems, 0, _visited, &converted, _error );

                if( successful == false )
                {
                    return false;
                }

                valueObject.set( keyString, converted );
                ++count;
            }

            _visited->erase( object );
            *_result = valueObject;

            return true;
        }

        uint64_t handle = this->retainHandle_( _value, _error );

        if( handle == 0 )
        {
            return false;
        }

        pybind::string_view typeValue = _value.repr_type();
        pybind::string_view reprValue = _value.repr();
        const Char * typeString = typeValue.c_str();
        const Char * reprString = reprValue.c_str();
        jpp::object reference = jpp::make_object();
        reference.set( "handle", (int64_t)handle );
        reference.set( "type", typeString );
        reference.set( "repr", reprString );

        if( _depth < _maxDepth )
        {
            PyObject * attributesObject = kernel->object_dir( object );

            if( attributesObject != nullptr && kernel->list_check( attributesObject ) == true )
            {
                pybind::object attributesOwner( kernel, attributesObject );
                size_t size = kernel->list_size( attributesObject );
                size_t begin = _depth == 0 && _start < size ? _start : (_depth == 0 ? size : 0);
                size_t remaining = size - begin;
                size_t count = remaining < _maxItems ? remaining : _maxItems;
                jpp::array attributes = jpp::make_array();

                for( size_t index = begin; index != begin + count; ++index )
                {
                    PyObject * attribute = kernel->list_getitem( attributesObject, index );

                    if( kernel->string_check( attribute ) == true )
                    {
                        const Char * attributeName = kernel->string_to_char( attribute );
                        attributes.push_back( attributeName );
                    }
                }

                reference.set( "attributes", attributes );
            }
        }

        *_result = reference;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MCPScriptContext::retainHandle_( const pybind::object & _value, String * const _error )
    {
        static const size_t MCP_SCRIPT_HANDLE_LIMIT = 4096;

        if( m_handles.size() >= MCP_SCRIPT_HANDLE_LIMIT )
        {
            *_error = "script object handle limit reached; call script_release";

            return 0;
        }

        uint64_t handle = m_nextHandle++;

        if( handle == 0 )
        {
            handle = m_nextHandle++;
        }

        m_handles.emplace( handle, _value );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptContext::resolveTarget( const jpp::object & _target, pybind::object * const _result, String * const _error ) const
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();

        if( _target.is_type_integer() == true )
        {
            int64_t signedHandle = _target;
            uint64_t handle = signedHandle > 0 ? (uint64_t)signedHandle : 0;
            Map<uint64_t, pybind::object>::const_iterator it = m_handles.find( handle );

            if( it == m_handles.end() )
            {
                *_error = "unknown or released script object handle";

                return false;
            }

            pybind::object value = it->second;
            *_result = value;

            return true;
        }

        if( _target.is_type_string() == false )
        {
            *_error = "script target must be a module path or object handle";

            return false;
        }

        String path = (const Char *)_target;
        bool sysExists = false;
        PyObject * sysObject = kernel->module_import( "sys", sysExists );
        pybind::object sys( kernel, sysObject );

        if( sysExists == false || sys.is_invalid() == true )
        {
            *_error = "sys module is unavailable";

            return false;
        }

        PyObject * sysValue = sys.ptr();
        PyObject * sysDict = kernel->module_dict( sysValue );
        PyObject * modules = kernel->dict_getstring( sysDict, "modules" );
        String moduleName = path;
        size_t attributeOffset = String::npos;

        for( ;; )
        {
            const Char * moduleNameString = moduleName.c_str();

            if( kernel->dict_existstring( modules, moduleNameString ) == true )
            {
                attributeOffset = moduleName.size();
                break;
            }

            size_t dot = moduleName.rfind( '.' );

            if( dot == String::npos )
            {
                *_error = "script module is not loaded";

                return false;
            }

            moduleName.resize( dot );
        }

        const Char * moduleNameString = moduleName.c_str();
        PyObject * module = kernel->dict_getstring( modules, moduleNameString );
        pybind::object current( kernel, module );
        size_t begin = attributeOffset;

        while( begin < path.size() )
        {
            if( path[begin] == '.' )
            {
                ++begin;
            }

            size_t end = path.find( '.', begin );
            size_t attributeSize = end == String::npos ? String::npos : end - begin;
            String attribute = path.substr( begin, attributeSize );
            const Char * attributeString = attribute.c_str();

            if( current.has_attr( attributeString ) == false )
            {
                *_error = "script target attribute does not exist";

                return false;
            }

            current = current.get_attr( attributeString );
            begin = end;
        }

        *_result = current;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptContext::resolveHandle( uint64_t _handle, pybind::object * const _result, String * const _error ) const
    {
        Map<uint64_t, pybind::object>::const_iterator it = m_handles.find( _handle );

        if( it == m_handles.end() )
        {
            *_error = "object handle does not exist";

            return false;
        }

        pybind::object value = it->second;
        *_result = value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptContext::retainObject( const pybind::object & _value, uint64_t * const _handle, String * const _error )
    {
        uint64_t handle = this->retainHandle_( _value, _error );

        if( handle == 0 )
        {
            return false;
        }

        *_handle = handle;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * MCPScriptContext::getScope( const Char * _scope, const Char * _module, String * const _error ) const
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();

        String scope = _scope;

        if( scope == "frame" )
        {
            *_error = "frame scope is only available during a debugger pause";

            return nullptr;
        }

        if( scope == "globals" )
        {
            if( m_globals.is_invalid() == true )
            {
                PyObject * globalsModule = kernel->module_init( "__mcp_globals__" );

                if( globalsModule == nullptr )
                {
                    *_error = "MCP globals are unavailable";

                    return nullptr;
                }

                m_globals = pybind::object( kernel, globalsModule );
            }

            PyObject * globalsValue = m_globals.ptr();
            PyObject * dict = kernel->module_dict( globalsValue );

            return dict;
        }

        const Char * moduleName = _module;

        if( moduleName == nullptr || moduleName[0] == '\0' )
        {
            moduleName = "__main__";
        }

        bool exists = false;
        PyObject * module = kernel->module_import( moduleName, exists );

        if( exists == false || module == nullptr )
        {
            *_error = "script module is not loaded";

            return nullptr;
        }

        PyObject * dict = kernel->module_dict( module );
        kernel->decref( module );

        return dict;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPScriptContext::setSource( const String & _module, SourceDesc && _source )
    {
        m_sources[_module] = StdUtility::move( _source );
    }
    //////////////////////////////////////////////////////////////////////////
    const MCPScriptContext::SourceDesc * MCPScriptContext::findSource( const String & _module ) const
    {
        Map<String, SourceDesc>::const_iterator it = m_sources.find( _module );

        if( it == m_sources.end() )
        {
            return nullptr;
        }

        const SourceDesc * source = &it->second;

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MCPScriptContext::releaseHandles( const jpp::array & _handles )
    {
        uint32_t released = 0;

        for( const jpp::object & handleObject : _handles )
        {
            int64_t signedHandle = handleObject;
            uint64_t handle = signedHandle > 0 ? (uint64_t)signedHandle : 0;
            size_t erased = m_handles.erase( handle );
            released += (uint32_t)erased;
        }

        return released;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
