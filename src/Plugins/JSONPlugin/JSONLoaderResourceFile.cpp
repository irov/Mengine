#include "JSONLoaderResourceFile.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceFile::JSONLoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceFile::~JSONLoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceFile::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceFile * resource = _factorable.getT<ResourceFile *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
