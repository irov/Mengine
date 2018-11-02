#include "LoaderResourceMusic.h"

#include "Interface/CodecServiceInterface.h"

#include "Engine/ResourceMusic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMusic::LoaderResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMusic::~LoaderResourceMusic()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceMusic::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceMusicPtr resource = stdex::intrusive_static_cast<ResourceMusicPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();

        resource->setFilePath( filePath );

        ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
            codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

        resource->setCodecType( codecType );

        ConstString converterType;
        metadata->get_File_Converter( &converterType );
        resource->setConverterType( converterType );

        bool external = false;
        metadata->get_File_External( &external );
        resource->setExternal( external );

        float volume = 1.f;
        metadata->get_DefaultVolume_Value( &volume );
        resource->setVolume( volume );

        return true;
    }
}
