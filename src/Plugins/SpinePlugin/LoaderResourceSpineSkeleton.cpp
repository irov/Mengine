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

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );

        metadata->getm_Atlas_Name( resource, &ResourceSpineSkeleton::setResourceSpineAtlasName );

        return true;
    }
}
