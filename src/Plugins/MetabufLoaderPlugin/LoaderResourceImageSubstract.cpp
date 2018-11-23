#include "LoaderResourceImageSubstract.h"

#include "Engine/ResourceImageSubstract.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstract::LoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstract::~LoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageSubstract::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSubstract * resource = stdex::intrusive_get<ResourceImageSubstract *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);

        const ConstString & resourceImageName = metadata->get_Image_Name();

        resource->setResourceImageName( resourceImageName );

        const mt::uv4f & uvImage = metadata->get_Image_UV();

        resource->setUVImage( uvImage );
        resource->setUVAlpha( uvImage );

        bool uvImageRotate = false;
        metadata->get_Image_UVRotate( &uvImageRotate );

        resource->setUVImageRotate( uvImageRotate );
        resource->setUVAlphaRotate( uvImageRotate );

        metadata->getm_Image_Alpha( resource, &ResourceImageSubstract::setAlpha );

        const mt::vec2f & maxSize = metadata->get_Image_MaxSize();
        resource->setMaxSize( maxSize );

        mt::vec2f size;
        if( metadata->get_Image_Size( &size ) == true )
        {
            resource->setSize( size );
        }
        else
        {
            resource->setSize( maxSize );
        }

        metadata->getm_Image_Offset( resource, &ResourceImageSubstract::setOffset );

        resource->correctUVTexture();

        return true;
    }
}
