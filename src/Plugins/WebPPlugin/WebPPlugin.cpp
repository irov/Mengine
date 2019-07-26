#include "WebPPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderWEBP.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( WebP, Mengine::WebPPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WebPPlugin::WebPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebPPlugin::~WebPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebPPlugin::_initializePlugin()
    {
        Helper::registerDecoder<ImageDecoderWEBP>( STRINGIZE_STRING_LOCAL( "webpImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "webp", STRINGIZE_STRING_LOCAL( "webpImage" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebPPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "webpImage" ) );
    }
}