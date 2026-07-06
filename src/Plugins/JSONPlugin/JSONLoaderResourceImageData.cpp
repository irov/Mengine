#include "JSONLoaderResourceImageData.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageData::JSONLoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageData::~JSONLoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageData::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageData * resource = _factorable.getT<ResourceImageData *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, true );

        mt::vec2f maxSize;
        if( Detail::getJSONMemberVec2Required( _json, "File", "MaxSize", &maxSize ) == false )
        {
            return false;
        }

        uint32_t width = (uint32_t)(maxSize.x + 0.5f);
        uint32_t height = (uint32_t)(maxSize.y + 0.5f);

        resource->setImageMaxSize( maxSize );
        resource->setImageWidth( width );
        resource->setImageHeight( height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
