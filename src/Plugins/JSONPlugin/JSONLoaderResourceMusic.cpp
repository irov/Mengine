#include "JSONLoaderResourceMusic.h"

#include "JSONLoaderResourceHelper.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceMusic::JSONLoaderResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceMusic::~JSONLoaderResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceMusic::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceMusic * resource = _factorable.getT<ResourceMusic *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, true );

        bool external = false;
        if( Helper::getJSONMemberBool( _json, "File", "External", &external ) == true )
        {
            resource->setExternal( external );
        }

        float defaultVolume = 1.f;
        if( Helper::getJSONMemberFloat( _json, "DefaultVolume", "Value", &defaultVolume ) == true )
        {
            resource->setVolume( defaultVolume );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
