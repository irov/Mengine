#include "MetabufLoaderResourceImageEmpty.h"

#include "Interface/CodecServiceInterface.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageEmpty::MetabufLoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageEmpty::~MetabufLoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceImageEmpty::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceImageEmpty * resource = _factorable.getT<ResourceImageEmpty *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageEmpty * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageEmpty *>(_meta);

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();
        resource->setMaxSize( maxSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
