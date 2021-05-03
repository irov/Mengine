#include "LoaderResourceSpineAtlasDefault.h"

#include "Interface/ResourceBankInterface.h"

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

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas::VectorMeta_Image & includes_images = metadata->get_Includes_Image();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlas::Meta_Image & meta_image : includes_images )
        {
            const ConstString & name = meta_image.get_Name();
            const ConstString & resourceName = meta_image.get_Resource();

            const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' category '%s' group '%s' invalid get image resource '%s'"
                , resource->getName().c_str()
                , content->getFileGroup()->getName().c_str()
                , resource->getGroupName().c_str()
                , resourceName.c_str()
            );

            resource->addResourceImageDesc( name, resourceImage );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
