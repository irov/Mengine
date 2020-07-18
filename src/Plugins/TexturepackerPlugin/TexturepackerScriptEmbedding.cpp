#include "TexturepackerScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptWrapperInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "ResourceTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ResourceImagePtr ResourceTexturepacker_getFrame( ResourceTexturepacker * _texturepacker, const ConstString & _name )
        {
            ResourceImagePtr resource;
            if( _texturepacker->findFrame( _name, &resource ) == false )
            {
                LOGGER_ERROR( "'%s' not found frame '%s'"
                    , _texturepacker->getName().c_str()
                    , _name.c_str()
                );

                return nullptr;
            }

            return resource;
        }
    }
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
        pybind::interface_<ResourceTexturepacker, pybind::bases<Resource>>( _kernel, "ResourceTexturepacker", false )
            .def_static( "getFrame", &Detail::ResourceTexturepacker_getFrame )
            .def( "getFrames", &ResourceTexturepacker::getFrames )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceTexturepacker>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceTexturepacker>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
    }
}
