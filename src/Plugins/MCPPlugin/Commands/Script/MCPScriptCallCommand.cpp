#include "MCPScriptCallCommand.h"

#include "../../Contexts/MCPHandlerRegistry.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Kernel/ConstStringHelper.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"
#include "pybind/kernel_interface.hpp"
#include "pybind/tuple.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptCallCommand::MCPScriptCallCommand( MCPHandlerRegistry * _handlerRegistry, MCPScriptContext * _scriptContext )
        : m_handlerRegistry( _handlerRegistry )
        , m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptCallCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object target;
        bool hasTarget = _request.params.exist( "target", &target );
        const Char * name = _request.params.get( "name", "" );
        bool hasName = name[0] != '\0';

        if( hasName == true && hasTarget == true )
        {
            _response->errorMessage = "script_call accepts either name or target, not both";

            return EMCPCommandStatus::FAILURE;
        }

        if( hasName == false && hasTarget == false )
        {
            _response->errorMessage = "script_call requires a name or target";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object argumentsObject;
        bool hasArguments = _request.params.exist( "arguments", &argumentsObject );

        if( hasName == true )
        {
            ConstString handlerName = Helper::stringizeString( name );
            MCPHandlerInterfacePtr handler = m_handlerRegistry->findHandler( handlerName );

            if( handler == nullptr )
            {
                _response->errorMessage = "script handler is not registered";

                return EMCPCommandStatus::FAILURE;
            }

            if( hasArguments == false )
            {
                argumentsObject = jpp::make_object();
            }

            try
            {
                jpp::object result;
                bool successful = handler->onMCPCall( argumentsObject, &result );

                if( successful == false )
                {
                    _response->errorMessage = "script handler call failed";

                    return EMCPCommandStatus::FAILURE;
                }

                _response->result = result;

                return EMCPCommandStatus::SUCCESS;
            }
            catch( const pybind::pybind_exception & ex )
            {
                _response->errorMessage = ex.what();

                return EMCPCommandStatus::FAILURE;
            }
        }

        jpp::object keywords;

        if( _request.params.exist( "keywords", &keywords ) == false || keywords.is_type_object() == false )
        {
            keywords = jpp::make_object();
        }

        if( hasArguments == true && argumentsObject.is_type_array() == false )
        {
            _response->errorMessage = "script_call target arguments must be an array";

            return EMCPCommandStatus::FAILURE;
        }

        try
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            String error;
            pybind::object callable;
            bool resolved = m_scriptContext->resolveTarget( target, &callable, &error );

            if( resolved == false || callable.is_callable() == false )
            {
                _response->errorMessage = resolved == false ? error : "script target is not callable";

                return EMCPCommandStatus::FAILURE;
            }

            if( hasArguments == false )
            {
                jpp::array arguments = jpp::make_array();
                argumentsObject = arguments.to_object();
            }

            jpp::array arguments( argumentsObject );
            size_t argumentCount = arguments.size();
            PyObject * tupleObject = kernel->tuple_new( argumentCount );
            pybind::tuple tuple( kernel, tupleObject );

            for( size_t index = 0; index != argumentCount; ++index )
            {
                pybind::object argument;
                bool converted = m_scriptContext->jsonToScript( arguments[index], &argument, &error );

                if( converted == false )
                {
                    _response->errorMessage = error;

                    return EMCPCommandStatus::FAILURE;
                }

                PyObject * tupleValue = tuple.ptr();
                PyObject * argumentValue = argument.ptr();
                bool assigned = kernel->tuple_setitem( tupleValue, index, argumentValue );

                if( assigned == false )
                {
                    _response->errorMessage = "failed to construct script arguments";

                    return EMCPCommandStatus::FAILURE;
                }
            }

            pybind::object keywordArguments;
            bool keywordsConverted = m_scriptContext->jsonToScript( keywords, &keywordArguments, &error );

            if( keywordsConverted == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            PyObject * callableValue = callable.ptr();
            PyObject * tupleValue = tuple.ptr();
            PyObject * keywordArgumentsValue = keywordArguments.ptr();
            PyObject * valueObject = kernel->ask_native_kw( callableValue, tupleValue, keywordArgumentsValue );
            pybind::object value( kernel, valueObject );
            jpp::object serialized;
            bool serializedSuccessful = m_scriptContext->scriptToJSON( value, 2, 100, 0, &serialized, &error );

            if( serializedSuccessful == false )
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
