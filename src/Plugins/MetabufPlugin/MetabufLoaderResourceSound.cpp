#include "MetabufLoaderResourceSound.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceSound::MetabufLoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceSound::~MetabufLoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceSound::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceSound * resource = _factorable.getT<ResourceSound *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSound *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );
        metadata->getm_File_Codec( contentPtr, &ContentInterface::setCodecType );
        metadata->getm_File_Converter( contentPtr, &ContentInterface::setConverterType );

        metadata->getm_DefaultVolume_Value( resource, &ResourceSound::setDefaultVolume );
        metadata->getm_IsStreamable_Value( resource, &ResourceSound::setStreamable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
