#include "JSONLoaderResourceSound.h"

#include "JSONLoaderResourceHelper.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSound::JSONLoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSound::~JSONLoaderResourceSound()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceSound::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceSound * resource = _factorable.getT<ResourceSound *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        float defaultVolume = 1.f;
        if( Helper::getJSONMemberFloat( _json, "DefaultVolume", "Value", &defaultVolume ) == true )
        {
            resource->setDefaultVolume( defaultVolume );
        }

        bool streamable = false;
        if( Helper::getJSONMemberBool( _json, "IsStreamable", "Value", &streamable ) == true )
        {
            resource->setStreamable( streamable );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
