#include "LoaderResourceImageSolid.h"

#include "Engine/ResourceImageSolid.h"

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
		ResourceImageSolidPtr resource = stdex::intrusive_static_cast<ResourceImageSolidPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSolid *>(_meta);

        const ColourValue & color = metadata->get_Color_Value();
        const mt::vec2f & size = metadata->get_Size_Value();

		resource->setColor( color );

		resource->setMaxSize( size );
		resource->setSize( size );

        return true;
    }
    
}