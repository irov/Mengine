#include "LoaderResourceImageSolid.h"

#include "Kernel/ResourceImageSolid.h"

#include "Metacode/Metacode.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSolid::LoaderResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSolid::~LoaderResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageSolid::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSolid * resource = _loadable.getT<ResourceImageSolid *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid *>(_meta);

        const Color & color = metadata->get_Color_Value();
        resource->setColor( color );

        const mt::vec2f & size = metadata->get_Size_Value();

        resource->setMaxSize( size );
        resource->setSize( size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}