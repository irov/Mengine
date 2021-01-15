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

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );
        metadata->getm_File_Codec( content, &ContentInterface::setCodecType );
        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );

        metadata->getm_DefaultVolume_Value( resource, &ResourceSound::setDefaultVolume );
        metadata->getm_IsStreamable_Value( resource, &ResourceSound::setStreamable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
