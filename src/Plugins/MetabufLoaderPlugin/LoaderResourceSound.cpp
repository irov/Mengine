#include "LoaderResourceSound.h"

#include "Engine/ResourceSound.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSound::LoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSound::~LoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSound::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSound * resource = stdex::intrusive_get<ResourceSound *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();
        resource->setFilePath( filePath );

        metadata->getm_File_Codec( resource, &ResourceSound::setCodecType );
        metadata->getm_File_Converter( resource, &ResourceSound::setConverterType );
        metadata->getm_DefaultVolume_Value( resource, &ResourceSound::setDefaultVolume );
        metadata->getm_IsStreamable_Value( resource, &ResourceSound::setStreamable );

        return true;
    }
}
