#include "LoaderResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceAstralax.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceAstralax::LoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceAstralax::~LoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceAstralax::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceAstralax * resource = stdex::intrusive_get<ResourceAstralax *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle *>(_meta);

        metadata->getm_File_Path( resource, &ResourceAstralax::setFilePath );
        metadata->getm_File_Converter( resource, &ResourceAstralax::setConverterType );

        uint32_t atlasCount = metadata->get_AtlasCount_Value();

        VectorResourceImages resourceImages( atlasCount );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::VectorMeta_Atlas & includes_atlas = metadata->get_Includes_Atlas();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::VectorMeta_Atlas::const_iterator
            it = includes_atlas.begin(),
            it_end = includes_atlas.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceParticle::Meta_Atlas & atlas = *it;

            uint32_t index = atlas.get_Index();
            const ConstString & resourceImageName = atlas.get_ResourceName();

            ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                ->getResourceReference( resourceImageName );

            if( resourceImage == nullptr )
            {
                LOGGER_ERROR( "resource %s container %s can't get atlas image %s"
                    , resource->getName().c_str()
                    , resource->getFilePath().c_str()
                    , resourceImageName.c_str()
                );

                return false;
            }

            resourceImages[index] = resourceImage;
        }

        resource->setResourceImages( resourceImages );

        return true;
    }
}
