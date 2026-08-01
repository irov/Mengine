#include "MCPScriptReloadModuleCommand.h"

#include "../../Contexts/MCPScriptContext.h"
#include "../../Contexts/MCPDebuggerContext.h"

#include "Config/StdUtility.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"
#include "pybind/module.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptReloadModuleCommand::MCPScriptReloadModuleCommand( MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext )
        : m_scriptContext( _scriptContext )
        , m_debuggerContext( _debuggerContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptReloadModuleCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        if( _request.attachment.empty() == true )
        {
            _response->errorMessage = "script reload requires a source attachment";

            return EMCPCommandStatus::FAILURE;
        }

        const Char * moduleName = _request.params.get( "module", "" );
        const Char * logicalPath = _request.params.get( "logicalPath", moduleName );
        const uint8_t * sourceData = _request.attachment.data();
        size_t sourceSize = _request.attachment.size();
        String source( reinterpret_cast<const Char *>(sourceData), sourceSize );

        if( source.find( '\0' ) != String::npos )
        {
            _response->errorMessage = "script source contains an embedded NUL byte";

            return EMCPCommandStatus::FAILURE;
        }

        try
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            bool exists = false;
            PyObject * liveObject = kernel->module_import( moduleName, exists );
            pybind::module live( kernel, liveObject );

            if( exists == false || live.is_invalid() == true )
            {
                _response->errorMessage = "script module is not loaded";

                return EMCPCommandStatus::FAILURE;
            }

            const Char * sourceString = source.c_str();
            PyObject * compiledObject = kernel->compile_string( sourceString, logicalPath );
            pybind::object compiled( kernel, compiledObject );

            if( compiled.is_invalid() == true )
            {
                _response->errorMessage = "script compilation failed";

                return EMCPCommandStatus::FAILURE;
            }

            pybind::object state = pybind::make_none_t( kernel );

            if( live.has_attr( "__mcp_before_reload__" ) == true )
            {
                pybind::object hook = live.get_attr( "__mcp_before_reload__" );
                pybind::object hookResult = hook.call();
                state = hookResult;
            }

            PyObject * liveObjectValue = live.ptr();
            PyObject * reloadedObject = kernel->module_reload_source( liveObjectValue, sourceString, logicalPath );
            pybind::object reloaded( kernel, reloadedObject );

            if( reloaded.is_invalid() == true )
            {
                _response->errorMessage = "candidate module execution failed; the live module dictionary was preserved";

                return EMCPCommandStatus::FAILURE;
            }

            if( live.has_attr( "__mcp_after_reload__" ) == true )
            {
                pybind::object hook = live.get_attr( "__mcp_after_reload__" );
                hook.call( state );
            }

            MCPScriptContext::SourceDesc sourceDesc;
            sourceDesc.logicalPath = logicalPath;
            sourceDesc.source = source;
            String moduleKey = moduleName;
            m_scriptContext->setSource( moduleKey, StdUtility::move( sourceDesc ) );

            PyObject * reloadedObjectValue = reloaded.ptr();
            bool identityPreserved = liveObjectValue == reloadedObjectValue;
            uint32_t sourceBytes = (uint32_t)sourceSize;
            uint32_t lineCount = 1;

            for( Char ch : source )
            {
                if( ch == '\n' )
                {
                    ++lineCount;
                }
            }

            jpp::array transferred = jpp::make_array();
            jpp::array disabled = jpp::make_array();
            String logicalFilename = logicalPath;
            m_debuggerContext->transferBreakpoints( logicalFilename, lineCount, &transferred, &disabled );
            _response->result.set( "module", moduleName );
            _response->result.set( "logicalFilename", logicalPath );
            _response->result.set( "reloaded", true );
            _response->result.set( "identityPreserved", identityPreserved );
            _response->result.set( "sourceBytes", sourceBytes );
            _response->result.set( "breakpointsTransferred", transferred );
            _response->result.set( "breakpointsDisabled", disabled );
            _response->result.set( "lineCount", lineCount );

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
