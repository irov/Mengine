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

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );
        metadata->getm_File_Codec( content.get(), &ContentInterface::setCodecType );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
