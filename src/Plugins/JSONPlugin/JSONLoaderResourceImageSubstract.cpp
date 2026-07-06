#include "JSONLoaderResourceImageSubstract.h"

#include "JSONLoaderResourceHelper.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSubstract::JSONLoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSubstract::~JSONLoaderResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageSubstract::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageSubstract * resource = _factorable.getT<ResourceImageSubstract *>();

        const ConstString & groupName = resource->getGroupName();

        ConstString resourceImageName;
        if( Helper::getJSONMemberConstString( _json, "Image", "Name", &resourceImageName ) == false )
        {
            return false;
        }

        ResourceImagePtr resourceImage;
        Detail::getJSONResourceImage( resource, groupName, resourceImageName, &resourceImage );

        resource->setResourceImage( resourceImage );

        mt::uv4f uvImage;
        if( Helper::getJSONMemberUV4f( _json, "Image", "UV", &uvImage ) == false )
        {
            return false;
        }

        resource->setUV( 0, uvImage );
        resource->setUV( 1, uvImage );

        bool uvImageRotate = false;
        Helper::getJSONMemberBool( _json, "Image", "UVRotate", &uvImageRotate );

        resource->setUVRotate( 0, uvImageRotate );
        resource->setUVRotate( 1, uvImageRotate );

        bool alpha = false;
        if( Helper::getJSONMemberBool( _json, "Image", "Alpha", &alpha ) == true )
        {
            resource->setAlpha( alpha );
        }

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
