#include "LoaderResourceSpineAtlasTexturepacker.h"

#include "ResourceSpineAtlasTexturepacker.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasTexturepacker::LoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasTexturepacker::~LoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSpineAtlasTexturepacker::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSpineAtlasTexturepacker * resource = stdex::intrusive_get<ResourceSpineAtlasTexturepacker *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker *>(_meta);

        metadata->getm_File_Path( resource, &ResourceSpineAtlasTexturepacker::setFilePath );
        metadata->getm_File_Converter( resource, &ResourceSpineAtlasTexturepacker::setConverterType );
        metadata->getm_Texturepacker_Name( resource, &ResourceSpineAtlasTexturepacker::setResourceTexturepackerName );

        return true;
    }
}
