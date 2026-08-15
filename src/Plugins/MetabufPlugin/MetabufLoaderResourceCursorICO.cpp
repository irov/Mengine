#include "MetabufLoaderResourceCursorICO.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceCursorICO::MetabufLoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceCursorICO::~MetabufLoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceCursorICO::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceCursorICO * resource = _factorable.getT<ResourceCursorICO *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
