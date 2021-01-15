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
        ResourceMusic * resource = stdex::intrusive_get<ResourceMusic *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic *>(_meta);

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );

        ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
            const FilePath & filePath = content->getFilePath();

            codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

        content->setCodecType( codecType );

        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );

        metadata->getm_File_External( resource, &ResourceMusic::setExternal );
        metadata->getm_DefaultVolume_Value( resource, &ResourceMusic::setVolume );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
