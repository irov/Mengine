#include "TexturepackerScriptEmbedding.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocument.h"

#include "ResourceTexturepacker.h"
#include "ResourceMultiTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

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
                LOGGER_ERROR( "resource texturepacker '%s' not found frame '%s'"
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
        pybind::interface_<ResourceTexturepacker, pybind::bases<Resource>>( _kernel, "ResourceTexturepacker", false )
            .def_static( "getFrame", &Detail::ResourceTexturepacker_getFrame )
            ;

        pybind::interface_<ResourceMultiTexturepacker, pybind::bases<Resource>>( _kernel, "ResourceMultiTexturepacker", false )
            .def( "addResourceTexturepacker", &ResourceMultiTexturepacker::addResourceTexturepacker )
            .def( "setStripFrameNameExtension", &ResourceMultiTexturepacker::setStripFrameNameExtension )
            .def( "getStripFrameNameExtension", &ResourceMultiTexturepacker::getStripFrameNameExtension )
            ;

        Helper::registerScriptWrapping<ResourceTexturepacker>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceMultiTexturepacker>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceTexturepacker>();
        _kernel->remove_scope<ResourceMultiTexturepacker>();

        Helper::unregisterScriptWrapping<ResourceTexturepacker>();
        Helper::unregisterScriptWrapping<ResourceMultiTexturepacker>();
    }
    //////////////////////////////////////////////////////////////////////////
}
