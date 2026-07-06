#include "JSONLoaderResourceImageSubstractRGBAndAlpha.h"

#include "JSONLoaderResourceHelper.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSubstractRGBAndAlpha::JSONLoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSubstractRGBAndAlpha::~JSONLoaderResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageSubstractRGBAndAlpha::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageSubstractRGBAndAlpha * resource = _factorable.getT<ResourceImageSubstractRGBAndAlpha *>();

        const ConstString & groupName = resource->getGroupName();

        resource->setAlpha( true );

        ConstString resourceImageRGBName;
        ConstString resourceImageAlphaName;

        if( Helper::getJSONMemberConstString( _json, "Image", "NameRGB", &resourceImageRGBName ) == false ||
            Helper::getJSONMemberConstString( _json, "Image", "NameAlpha", &resourceImageAlphaName ) == false )
        {
            return false;
        }

        ResourceImagePtr resourceImageRGB;
        ResourceImagePtr resourceImageAlpha;

        Detail::getJSONResourceImage( resource, groupName, resourceImageRGBName, &resourceImageRGB );
        Detail::getJSONResourceImage( resource, groupName, resourceImageAlphaName, &resourceImageAlpha );

        resource->setResourceImageRGB( resourceImageRGB );
        resource->setResourceImageAlpha( resourceImageAlpha );

        mt::uv4f uvImage;
        mt::uv4f uvAlpha;

        if( Helper::getJSONMemberUV4f( _json, "Image", "UVRGB", &uvImage ) == false ||
            Helper::getJSONMemberUV4f( _json, "Image", "UVAlpha", &uvAlpha ) == false )
        {
            return false;
        }

        resource->setUV( 0, uvImage );
        resource->setUV( 1, uvAlpha );

        bool uvImageRotate = false;
        Helper::getJSONMemberBool( _json, "Image", "UVRGBRotate", &uvImageRotate );
        resource->setUVRotate( 0, uvImageRotate );

        bool uvAlphaRotate = false;
        Helper::getJSONMemberBool( _json, "Image", "UVAlphaRotate", &uvAlphaRotate );
        resource->setUVRotate( 1, uvAlphaRotate );

        mt::vec2f maxSize;
        if( Helper::getJSONMemberVec2f( _json, "Image", "MaxSize", &maxSize ) == false )
        {
            return false;
        }

        resource->setMaxSize( maxSize );

        mt::vec2f size;
        if( Helper::getJSONMemberVec2f( _json, "Image", "Size", &size ) == true )
        {
            resource->setSize( size );
        }
        else
        {
            resource->setSize( maxSize );
        }

        mt::vec2f offset;
        if( Helper::getJSONMemberVec2f( _json, "Image", "Offset", &offset ) == true )
        {
            resource->setOffset( offset );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
