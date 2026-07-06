#include "JSONLoaderResourceVideo.h"

#include "ResourceVideo.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceVideo::JSONLoaderResourceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceVideo::~JSONLoaderResourceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceVideo::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceVideo * resource = _factorable.getT<ResourceVideo *>();

        const ContentInterfacePtr & content = resource->getContent();

        FilePath filePath;
        if( Helper::getJSONMemberFilePath( _json, "File", "Path", &filePath ) == true )
        {
            content->setFilePath( filePath );
        }

        ConstString codecType;
        if( Helper::getJSONMemberConstString( _json, "File", "Codec", &codecType ) == true )
        {
            content->setCodecType( codecType );
        }

        ConstString converterType;
        if( Helper::getJSONMemberConstString( _json, "File", "Converter", &converterType ) == true )
        {
            content->setConverterType( converterType );
        }

        bool noExist = false;
        if( Helper::getJSONMemberBool( _json, "File", "NoExist", &noExist ) == true )
        {
            content->setValidNoExist( noExist );
        }

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

        bool noSeek = false;
        if( Helper::getJSONMemberBool( _json, "File", "NoSeek", &noSeek ) == true )
        {
            resource->setNoSeek( noSeek );
        }

        float frameRate = 0.f;
        if( Helper::getJSONMemberFloat( _json, "File", "FrameRate", &frameRate ) == true )
        {
            resource->setFrameRate( frameRate );
        }

        float duration = 0.f;
        if( Helper::getJSONMemberFloat( _json, "File", "Duration", &duration ) == true )
        {
            resource->setDuration( duration );
        }

        float resize = 0.f;
        if( Helper::getJSONMemberFloat( _json, "File", "Resize", &resize ) == true )
        {
            resource->setResize( resize );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
