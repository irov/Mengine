#include "MetabufLoaderResourceTiledMap.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceTiledMap::MetabufLoaderResourceTiledMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceTiledMap::~MetabufLoaderResourceTiledMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceTiledMap::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceTiledMap * resource = _factorable.getT<ResourceTiledMap *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTiledMap * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTiledMap *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();
        ContentInterface * contentPtr = content.get();

        metadata->getm_File_Path( contentPtr, &ContentInterface::setFilePath );
        metadata->getm_File_Converter( contentPtr, &ContentInterface::setConverterType );
        metadata->getm_File_NoExist( contentPtr, &ContentInterface::setValidNoExist );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
