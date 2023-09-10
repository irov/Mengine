#include "LoaderResourceInternalObject.h"

#include "ResourceInternalObject.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceInternalObject::LoaderResourceInternalObject()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceInternalObject::~LoaderResourceInternalObject()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceInternalObject::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceInternalObject * resource = _factorable.getT<ResourceInternalObject *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceInternalObject * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceInternalObject *>(_meta);

        const ConstString & internalName = metadata->get_Internal_Name();
        const ConstString & internalGroup = metadata->get_Internal_Group();

        resource->setInternalName( internalName );
        resource->setInternalGroup( internalGroup );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
