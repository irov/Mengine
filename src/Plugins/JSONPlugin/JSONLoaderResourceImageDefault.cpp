#include "JSONLoaderResourceImageDefault.h"

#include "JSONLoaderResourceHelper.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageDefault::JSONLoaderResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageDefault::~JSONLoaderResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageDefault::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageDefault * resource = _factorable.getT<ResourceImageDefault *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, true );

        bool alpha = false;
        if( Helper::getJSONMemberBool( _json, "File", "Alpha", &alpha ) == true )
        {
            resource->setAlpha( alpha );
        }

        bool premultiply = false;
        if( Helper::getJSONMemberBool( _json, "File", "Premultiply", &premultiply ) == true )
        {
            resource->setPremultiply( premultiply );
        }

        bool trimAtlas = false;
        if( Helper::getJSONMemberBool( _json, "File", "TrimAtlas", &trimAtlas ) == true )
        {
            resource->setTrimAtlas( trimAtlas );
        }

        mt::vec2f maxSize;
        if( Detail::getJSONMemberVec2Required( _json, "File", "MaxSize", &maxSize ) == false )
        {
            return false;
        }

        resource->setMaxSize( maxSize );

        mt::vec2f size;
        if( Helper::getJSONMemberVec2f( _json, "File", "Size", &size ) == true )
        {
            resource->setSize( size );
        }
        else
        {
            resource->setSize( maxSize );
        }

        mt::vec2f offset;
        if( Helper::getJSONMemberVec2f( _json, "File", "Offset", &offset ) == true )
        {
            resource->setOffset( offset );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
