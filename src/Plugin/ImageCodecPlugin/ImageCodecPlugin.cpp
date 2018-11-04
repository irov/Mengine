#include "ImageCodecPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/StringizeInterface.h"

#include "ImageDecoderPNG.h"
#include "ImageDecoderJPEG.h"

#ifdef MENGINE_SUPPORT_DECODER_WEBP
#		include "ImageDecoderWEBP.h"
#endif

#include "ImageDecoderDDS.h"
#include "ImageEncoderDDS.h"

#include "ImageDecoderPVRTC.h"
#include "ImageDecoderETC1.h"

#include "ImageDecoderHTF.h"
#include "ImageEncoderHTF.h"
#include "ImageDecoderACF.h"
#include "ImageEncoderACF.h"

#include "ImageEncoderPNG.h"
#include "ImageEncoderJPEG.h"

#include "PickDecoderHIT.h"
#include "PickEncoderHIT.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ImageCodec, Mengine::ImageCodecPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageCodecPlugin::ImageCodecPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageCodecPlugin::~ImageCodecPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageCodecPlugin::_initialize()
    {
        Helper::registerDecoder<ImageDecoderPNG>( "pngImage" );
        Helper::registerDecoder<ImageDecoderJPEG>( "jpegImage" );

#ifdef MENGINE_SUPPORT_DECODER_WEBP
        Helper::registerDecoder<ImageDecoderWEBP>( "webpImage" );
#endif

        Helper::registerDecoder<ImageDecoderPVRTC>( "pvrImage" );
        Helper::registerDecoder<ImageDecoderETC1>( "etcImage" );
        Helper::registerDecoder<ImageDecoderDDS>( "ddsImage" );

        Helper::registerDecoder<ImageDecoderHTF>( "htfImage" );
        Helper::registerDecoder<ImageDecoderACF>( "acfImage" );

        CODEC_SERVICE()
            ->registerCodecExt( "png", STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "PNG", STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "jpg", STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "jpeg", STRINGIZE_STRING_LOCAL( "jpegImage" ) );

#ifdef MENGINE_SUPPORT_DECODER_WEBP
        CODEC_SERVICE()
            ->registerCodecExt( "webp", STRINGIZE_STRING_LOCAL( "webpImage" ) );
#endif

        CODEC_SERVICE()
            ->registerCodecExt( "pvr", STRINGIZE_STRING_LOCAL( "pvrImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "dds", STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "etc", STRINGIZE_STRING_LOCAL( "etcImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "crn", STRINGIZE_STRING_LOCAL( "crnImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "htf", STRINGIZE_STRING_LOCAL( "htfImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "acf", STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::registerEncoder<ImageEncoderPNG>( "pngImage" );
        Helper::registerEncoder<ImageEncoderJPEG>( "jpegImage" );
        Helper::registerEncoder<ImageEncoderHTF>( "htfImage" );
        Helper::registerEncoder<ImageEncoderACF>( "acfImage" );

        Helper::registerEncoder<ImageEncoderDDS>( "ddsImage" );

        Helper::registerDecoder<PickDecoderHIT>( "hitPick" );
        Helper::registerEncoder<PickEncoderHIT>( "hitPick" );

        CODEC_SERVICE()
            ->registerCodecExt( "hit", STRINGIZE_STRING_LOCAL( "hitPick" ) );        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageCodecPlugin::_finalize()
    {
        Helper::unregisterDecoder( "pngImage" );
        Helper::unregisterDecoder( "jpegImage" );

#ifdef MENGINE_SUPPORT_DECODER_WEBP
        Helper::unregisterDecoder( "webpImage" );
#endif

        Helper::unregisterDecoder( "pvrImage" );
        Helper::unregisterDecoder( "etcImage" );
        Helper::unregisterDecoder( "ddsImage" );

        Helper::unregisterDecoder( "htfImage" );
        Helper::unregisterDecoder( "acfImage" );
        Helper::unregisterEncoder( "pngImage" );
        Helper::unregisterEncoder( "jpegImage" );
        Helper::unregisterEncoder( "htfImage" );
        Helper::unregisterEncoder( "acfImage" );

        Helper::unregisterEncoder( "ddsImage" );

        Helper::unregisterDecoder( "hitPick" );
        Helper::unregisterEncoder( "hitPick" );
    }
}