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
        ResourceSoundPtr resource = stdex::intrusive_static_cast<ResourceSoundPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();
        resource->setFilePath( filePath );

        ConstString codecType;
        metadata->get_File_Codec( &codecType );
        resource->setCodecType( codecType );

        ConstString converterType;
        metadata->get_File_Converter( &converterType );
        resource->setConverterType( converterType );

        float defaultVolume;
        if( metadata->get_DefaultVolume_Value( &defaultVolume ) == true )
        {
            resource->setDefaultVolume( defaultVolume );
        }

        bool isStreamable;
        if( metadata->get_IsStreamable_Value( &isStreamable ) == true )
        {
            resource->setStreamable( isStreamable );
        }

        return true;
    }
}
