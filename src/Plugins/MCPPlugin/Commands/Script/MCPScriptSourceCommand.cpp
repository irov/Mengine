#include "MCPScriptSourceCommand.h"

#include "../../Contexts/MCPScriptContext.h"

#include "Config/StdString.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/kernel_interface.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptSourceCommand::MCPScriptSourceCommand( MCPScriptContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptSourceCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * moduleValue = _request.params.get( "module", "" );
        String module = moduleValue;
        const MCPScriptContext::SourceDesc * source = m_context->findSource( module );
        _response->result.set( "module", moduleValue );

        if( source != nullptr )
        {
            _response->result.set( "kind", "session_source" );
            _response->result.set( "logicalFilename", source->logicalPath );
            _response->result.set( "source", source->source );
            _response->result.set( "sourceAvailable", true );

            return EMCPCommandStatus::SUCCESS;
        }

        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        bool exists = false;
        PyObject * moduleObjectValue = kernel->module_import( moduleValue, exists );
        pybind::object moduleObject( kernel, moduleObjectValue );

        if( exists == false || moduleObject.is_invalid() == true )
        {
            _response->errorMessage = "script module is not loaded";

            return EMCPCommandStatus::FAILURE;
        }

        String logicalFilename = module;

        if( moduleObject.has_attr( "__file__" ) == true )
        {
            pybind::object filename = moduleObject.get_attr( "__file__" );

            if( filename.is_string() == true )
            {
                logicalFilename = (const Char *)filename.extract();
            }
        }

        const Char * logicalFilenameString = logicalFilename.c_str();
        bool archive = StdString::strstr( logicalFilenameString, ".pyz" ) != nullptr;
        const Char * kind = archive == true ? "pyz" : "packaged";
        jpp::array lineMap = jpp::make_array();
        _response->result.set( "kind", kind );
        _response->result.set( "logicalFilename", logicalFilename );
        _response->result.set( "sourceAvailable", false );
        _response->result.set( "lineMap", lineMap );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
