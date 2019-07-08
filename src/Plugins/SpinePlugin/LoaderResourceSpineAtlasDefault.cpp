#include "LoaderResourceSpineAtlasDefault.h"

#include "ResourceSpineAtlasDefault.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasDefault::LoaderResourceSpineAtlasDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasDefault::~LoaderResourceSpineAtlasDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSpineAtlasDefault::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSpineAtlasDefault * resource = stdex::intrusive_get<ResourceSpineAtlasDefault *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas *>(_meta);

        metadata->getm_File_Path( resource, &ResourceSpineAtlasDefault::setFilePath );
        metadata->getm_File_Converter( resource, &ResourceSpineAtlasDefault::setConverterType );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas::VectorMeta_Image & includes_images = metadata->get_Includes_Image();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image & meta_image : includes_images )
        {
            const ConstString & name = meta_image.get_Name();
            const ConstString & resourceName = meta_image.get_Resource();

            resource->addResourceImageDesc( name, resourceName );
        }

        return true;
    }
}
