#include "TexturepackerScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptWrapperInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "ResourceTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TexturepackerScriptEmbedding::TexturepackerScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TexturepackerScriptEmbedding::~TexturepackerScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TexturepackerScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceTexturepacker, pybind::bases<Resource> >( _kernel, "ResourceTexturepacker", false )
            .def( "getFrame", &ResourceTexturepacker::getFrame )
            .def( "getFrames", &ResourceTexturepacker::getFrames )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceTexturepacker> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceTexturepacker>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
    }
}
