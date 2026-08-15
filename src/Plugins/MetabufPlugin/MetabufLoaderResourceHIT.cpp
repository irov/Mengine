#include "MetabufLoaderResourceHIT.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceHIT::MetabufLoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceHIT::~MetabufLoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceHIT::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceHIT * resource = _factorable.getT<ResourceHIT *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );
        metadata->getm_File_Converter( contentPtr, &ContentInterface::setConverterType );
        metadata->getm_File_Codec( contentPtr, &ContentInterface::setCodecType );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
