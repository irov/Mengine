#include "LoaderResourceSpineSkeleton.h"

#include "ResourceSpineSkeleton.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineSkeleton::LoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineSkeleton::~LoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSpineSkeleton::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSpineSkeleton * resource = stdex::intrusive_get<ResourceSpineSkeleton *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton *>(_meta);

        metadata->getm_File_Path( resource, &Content::setFilePath );
        metadata->getm_File_Converter( resource, &Content::setConverterType );

        metadata->getm_Atlas_Name( resource, &ResourceSpineSkeleton::setResourceSpineAtlasName );

        return true;
    }
}
