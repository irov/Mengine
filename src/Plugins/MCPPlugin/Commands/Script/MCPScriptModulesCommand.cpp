#include "MCPScriptModulesCommand.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"
#include "pybind/kernel_interface.hpp"
#include "pybind/object.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptModulesCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        try
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            bool exists = false;
            PyObject * sysObject = kernel->module_import( "sys", exists );
            pybind::object sys( kernel, sysObject );

            if( exists == false || sys.is_invalid() == true )
            {
                _response->errorMessage = "sys module is unavailable";

                return EMCPCommandStatus::FAILURE;
            }

            PyObject * sysValue = sys.ptr();
            PyObject * sysDict = kernel->module_dict( sysValue );
            PyObject * modules = kernel->dict_getstring( sysDict, "modules" );
            size_t position = 0;
            PyObject * key = nullptr;
            PyObject * value = nullptr;
            jpp::array names = jpp::make_array();

            while( kernel->dict_next( modules, &position, &key, &value ) == true )
            {
                MENGINE_UNUSED( value );

                if( kernel->string_check( key ) == true )
                {
                    const Char * name = kernel->string_to_char( key );
                    names.push_back( name );
                }
            }

            size_t countValue = names.size();
            uint32_t count = (uint32_t)countValue;
            _response->result.set( "modules", names );
            _response->result.set( "count", count );

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
