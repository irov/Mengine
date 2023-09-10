#include "MetabufLoaderResourceImageSubstractRGBAndAlpha.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageSubstractRGBAndAlpha::MetabufLoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageSubstractRGBAndAlpha::~MetabufLoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceImageSubstractRGBAndAlpha::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSubstractRGBAndAlpha * resource = _factorable.getT<ResourceImageSubstractRGBAndAlpha *>();

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha *>(_meta);

        resource->setAlpha( true );

        const ConstString & resourceImageRGBName = metadata->get_Image_NameRGB();
        const ConstString & resourceImageAlphaName = metadata->get_Image_NameAlpha();

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourceImagePtr & resourceImageRGB = resourceBank->getResource( groupName, resourceImageRGBName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImageRGB, "'%s' group '%s' invalid get image RGB resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceImageRGBName.c_str()
        );

        const ResourceImagePtr & resourceImageAlpha = resourceBank->getResource( groupName, resourceImageAlphaName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImageAlpha, "'%s' group '%s' invalid get image Alpha resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceImageAlphaName.c_str()
        );

        resource->setResourceImageRGB( resourceImageRGB );
        resource->setResourceImageAlpha( resourceImageAlpha );

        const mt::uv4f & uvImage = metadata->get_Image_UVRGB();
        const mt::uv4f & uvAlpha = metadata->get_Image_UVAlpha();

        resource->setUVImage( uvImage );
        resource->setUVAlpha( uvAlpha );

        metadata->getm_Image_UVRGBRotate( resource, &ResourceImageSubstractRGBAndAlpha::setUVImageRotate );
        metadata->getm_Image_UVAlphaRotate( resource, &ResourceImageSubstractRGBAndAlpha::setUVAlphaRotate );

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

        metadata->getm_Image_Offset( resource, &ResourceImageSubstractRGBAndAlpha::setOffset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
