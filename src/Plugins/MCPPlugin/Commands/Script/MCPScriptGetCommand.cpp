#include "MCPScriptGetCommand.h"

#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"
#include "pybind/kernel_interface.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptGetCommand::MCPScriptGetCommand( MCPScriptContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptGetCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object target;
        jpp::object key;

        if( _request.params.exist( "target", &target ) == false || _request.params.exist( "key", &key ) == false )
        {
            _response->errorMessage = "script_get requires target and key";

            return EMCPCommandStatus::FAILURE;
        }

        try
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            String error;
            pybind::object object;
            bool resolved = m_context->resolveTarget( target, &object, &error );

            if( resolved == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            pybind::object value;

            if( key.is_type_string() == true )
            {
                const Char * name = key;

                if( object.is_dict() == true )
                {
                    PyObject * objectValue = object.ptr();
                    PyObject * item = kernel->dict_getstring( objectValue, name );

                    if( item == nullptr )
                    {
                        _response->errorMessage = "dictionary key does not exist";

                        return EMCPCommandStatus::FAILURE;
                    }

                    value = pybind::object( kernel, item );
                }
                else
                {
                    if( object.has_attr( name ) == false )
                    {
                        _response->errorMessage = "attribute does not exist";

                        return EMCPCommandStatus::FAILURE;
                    }

                    value = object.get_attr( name );
                }
            }
            else if( key.is_type_integer() == true && object.is_list() == true )
            {
                uint32_t index = key;
                PyObject * objectValue = object.ptr();
                size_t size = kernel->list_size( objectValue );

                if( index >= size )
                {
                    _response->errorMessage = "list index is out of range";

                    return EMCPCommandStatus::FAILURE;
                }

                PyObject * item = kernel->list_getitem( objectValue, index );
                value = pybind::object( kernel, item );
            }
            else
            {
                _response->errorMessage = "unsupported script key for target";

                return EMCPCommandStatus::FAILURE;
            }

            jpp::object serialized;
            bool successful = m_context->scriptToJSON( value, 2, 100, 0, &serialized, &error );

            if( successful == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            _response->result.set( "value", serialized );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
