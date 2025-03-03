#include "MetabufLoaderResourceCursorSystem.h"

#include "Interface/ContentInterface.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceCursorSystem::MetabufLoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceCursorSystem::~MetabufLoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceCursorSystem::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceCursorSystem * resource = _factorable.getT<ResourceCursorSystem *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}