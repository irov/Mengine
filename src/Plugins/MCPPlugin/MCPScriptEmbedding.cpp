#include "MCPScriptEmbedding.h"

#include "MCPInterface.h"
#include "MCPPythonHandler.h"

#include "Environment/Python/PythonDocument.h"

#include "Kernel/FactorableUnique.h"

#include "pybind/function.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static MCPHandlerInterfacePtr mcpAddHandler( const ConstString & _name, const pybind::object & _callback, const pybind::args & _args )
        {
            MCPHandlerInterfacePtr result = nullptr;

            if( _name.empty() == true )
            {
                pybind::throw_exception( "MCP handler name must not be empty" );

                return result;
            }

            if( _callback.is_callable() == false )
            {
                const Char * name = _name.c_str();

                pybind::throw_exception( "MCP handler '%s' callback must be callable"
                    , name
                );

                return result;
            }

            MCPPythonHandlerPtr handler = Helper::makeFactorableUnique<MCPPythonHandler>( MENGINE_DOCUMENT_PYTHON, _callback, _args );
            bool successful = MCP_SERVICE()->addHandler( _name, handler );

            if( successful == false )
            {
                const Char * name = _name.c_str();

                pybind::throw_exception( "MCP handler '%s' is already registered"
                    , name
                );

                return result;
            }

            result = handler;

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void mcpRemoveHandler( const MCPHandlerInterfacePtr & _handler )
        {
            MCP_SERVICE()->removeHandler( _handler );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPScriptEmbedding::MCPScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPScriptEmbedding::~MCPScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        bool successful = true;

        pybind::interface_<MCPHandlerInterface, pybind::bases<Mixin>>( _kernel, "MCPHandler" )
            ;

        pybind::def_function_args( _kernel, "mcpAddHandler", &Detail::mcpAddHandler );
        pybind::def_function( _kernel, "mcpRemoveHandler", &Detail::mcpRemoveHandler );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "mcpAddHandler", nullptr );
        _kernel->remove_from_module( "mcpRemoveHandler", nullptr );

        _kernel->remove_scope<MCPHandlerInterface>();
    }
    //////////////////////////////////////////////////////////////////////////
}
