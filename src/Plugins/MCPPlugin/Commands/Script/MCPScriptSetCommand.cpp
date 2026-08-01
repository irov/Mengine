#include "MCPScriptSetCommand.h"

#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"
#include "pybind/kernel_interface.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptSetCommand::MCPScriptSetCommand( MCPScriptContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptSetCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object target;
        jpp::object key;
        jpp::object jsonValue;

        if( _request.params.exist( "target", &target ) == false || _request.params.exist( "key", &key ) == false || _request.params.exist( "value", &jsonValue ) == false )
        {
            _response->errorMessage = "script_set requires target, key, and value";

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
            bool converted = m_context->jsonToScript( jsonValue, &value, &error );

            if( converted == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            bool successful = false;
            PyObject * objectValue = object.ptr();
            PyObject * scriptValue = value.ptr();

            if( key.is_type_string() == true )
            {
                const Char * name = key;

                if( object.is_dict() == true )
                {
                    successful = kernel->dict_setstring( objectValue, name, scriptValue );
                }
                else
                {
                    successful = kernel->set_attrstring( objectValue, name, scriptValue );
                }
            }
            else if( key.is_type_integer() == true && object.is_list() == true )
            {
                uint32_t index = key;
                size_t size = kernel->list_size( objectValue );

                if( index < size )
                {
                    successful = kernel->list_setitem( objectValue, index, scriptValue );
                }
            }

            if( successful == false )
            {
                _response->errorMessage = "failed to set script value";

                return EMCPCommandStatus::FAILURE;
            }

            _response->result.set( "updated", true );

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
