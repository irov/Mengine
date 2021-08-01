#include "LoaderResourceImageEmpty.h"

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ResourceImageEmpty.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageEmpty::LoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageEmpty::~LoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageEmpty::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageEmpty * resource = stdex::intrusive_get<ResourceImageEmpty *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageEmpty * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageEmpty *>(_meta);

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();
        resource->setMaxSize( maxSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
