#include "LoaderResourceImageSubstractRGBAndAlpha.h"

#include "Engine/ResourceImageSubstractRGBAndAlpha.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstractRGBAndAlpha::LoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSubstractRGBAndAlpha::~LoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageSubstractRGBAndAlpha::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSubstractRGBAndAlphaPtr resource = stdex::intrusive_static_cast<ResourceImageSubstractRGBAndAlphaPtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha *>(_meta);

        resource->setAlpha( true );

        const ConstString & resourceImageRGBName = metadata->get_Image_NameRGB();
        const ConstString & resourceImageAlphaName = metadata->get_Image_NameAlpha();

        resource->setResourceImageRGBName( resourceImageRGBName );
        resource->setResourceImageAlphaName( resourceImageAlphaName );

        const mt::uv4f & uvImage = metadata->get_Image_UVRGB();
        const mt::uv4f & uvAlpha = metadata->get_Image_UVAlpha();

        resource->setUVImage( uvImage );
        resource->setUVAlpha( uvAlpha );

        bool uvImageRotate = false;
        metadata->get_Image_UVRGBRotate( &uvImageRotate );
        resource->setUVImageRotate( uvImageRotate );

        bool m_uvAlphaRotate = false;
        metadata->get_Image_UVAlphaRotate( &uvAlphaRotate );
        resource->setUVAlphaRotate( uvAlphaRotate );

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

        mt::vec2f offset;
        metadata->get_Image_Offset( &offset );

        return true;
    }    
}
