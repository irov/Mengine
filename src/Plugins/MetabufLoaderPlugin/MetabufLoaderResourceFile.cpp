#include "MetabufLoaderResourceFile.h"

#include "Engine/ResourceFile.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceFile::MetabufLoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceFile::~MetabufLoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceFile::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceFile * resource = _factorable.getT<ResourceFile *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
