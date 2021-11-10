#include "TexturepackerScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourceTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

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
    bool TexturepackerScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Texturepacker", true );

        pybind::interface_<ResourceTexturepacker, pybind::bases<Resource>>( _kernel, "ResourceTexturepacker", false )
            .def_static( "getFrame", &Detail::ResourceTexturepacker_getFrame )
            ;

        Helper::registerScriptWrapping<ResourceTexturepacker>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceTexturepacker>();

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
