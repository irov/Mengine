#include "JSONLoaderResourceCursorSystem.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceCursorSystem::JSONLoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceCursorSystem::~JSONLoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceCursorSystem::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceCursorSystem * resource = _factorable.getT<ResourceCursorSystem *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
