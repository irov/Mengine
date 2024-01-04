#include "MetabufLoaderResourceSound.h"

#include "Kernel/ResourceSound.h"

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

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_Codec( content.get(), &ContentInterface::setCodecType );
        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );

        metadata->getm_DefaultVolume_Value( resource, &ResourceSound::setDefaultVolume );
        metadata->getm_IsStreamable_Value( resource, &ResourceSound::setStreamable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
