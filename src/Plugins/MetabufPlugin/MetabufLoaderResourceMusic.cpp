#include "MetabufLoaderResourceMusic.h"

#include "Interface/CodecServiceInterface.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceMusic::MetabufLoaderResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceMusic::~MetabufLoaderResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceMusic::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceMusic * resource = _factorable.getT<ResourceMusic *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMusic *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );

        ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
            const FilePath & filePath = content->getFilePath();

            codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

        content->setCodecType( codecType );

        metadata->getm_File_Converter( contentPtr, &ContentInterface::setConverterType );

        metadata->getm_File_External( resource, &ResourceMusic::setExternal );
        metadata->getm_DefaultVolume_Value( resource, &ResourceMusic::setVolume );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
