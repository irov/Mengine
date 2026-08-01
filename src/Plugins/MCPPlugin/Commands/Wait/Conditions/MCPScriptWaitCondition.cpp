#include "MCPScriptWaitCondition.h"

#include "../../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptWaitCondition::MCPScriptWaitCondition( MCPScriptContext * _scriptContext )
        : m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        const Char * expression = _request.condition.get( "expression", "" );
        const Char * scope = _request.condition.get( "scope", "module" );
        const Char * module = _request.condition.get( "module", "" );

        try
        {
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            String error;
            PyObject * globals = m_scriptContext->getScope( scope, module, &error );

            if( globals == nullptr )
            {
                _response->errorMessage = error;

                return false;
            }

            PyObject * valueObject = kernel->eval_string( expression, globals, globals );
            pybind::object value( kernel, valueObject );

            if( value.is_invalid() == true )
            {
                _response->errorMessage = "wait_for script expression failed";

                return false;
            }

            PyObject * scriptValue = value.ptr();
            bool satisfied = kernel->is_true( scriptValue );
            _response->satisfied = satisfied;
            _response->details.set( "value", satisfied );

            return true;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
