#include "MetabufLoaderResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceAstralax.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceAstralax::MetabufLoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceAstralax::~MetabufLoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceAstralax::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceAstralax * resource = _factorable.getT<ResourceAstralax *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAstralax * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAstralax *>(_meta);

        const ConstString & groupName = resource->getGroupName();
        const ConstString & resourceName = resource->getName();
        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();
        const PathString contentFullPath = Helper::getContentFullPath( content );

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );
        metadata->getm_File_Converter( contentPtr, &ContentInterface::setConverterType );

        uint32_t atlasCount = metadata->get_AtlasCount_Value();
        VectorResourceImages resourceImages( atlasCount );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAstralax::VectorMeta_Atlas & includes_atlas = metadata->get_Includes_Atlas();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceAstralax::Meta_Atlas & atlas : includes_atlas )
        {
            uint32_t index = atlas.get_Index();

            if( index >= atlasCount )
            {
                LOGGER_ERROR( "resource '%s' atlas index [%u] exceeds atlas count [%u]"
                    , resourceName.c_str()
                    , index
                    , atlasCount
                );

                return false;
            }

            const ConstString & resourceImageName = atlas.get_ResourceName();

            ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                ->getResourceReference( groupName, resourceImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "resource '%s' container '%s' can't get atlas image '%s'"
                , resourceName.c_str()
                , contentFullPath.c_str()
                , resourceImageName.c_str()
            );

            resourceImages[index] = resourceImage;
        }

        resource->setResourceImages( resourceImages );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
