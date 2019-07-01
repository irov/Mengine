#include "JSONScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "ResourceJSON.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_getJSON( ResourceJSON * _resource )
        {
            const jpp::object& root = _resource->getJSON();
            (void)root;

            return nullptr;
        }
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
    bool JSONScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceJSON, pybind::bases<Resource, Content> >( _kernel, "ResourceJSON", false )
            .def_static( "getJSON", &Helper::s_getJSON )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceJSON> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceJSON>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );
    }
}
