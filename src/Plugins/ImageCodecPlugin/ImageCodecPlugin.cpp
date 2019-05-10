#include "ImageCodecPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "ImageDecoderPNG.h"
#include "ImageDecoderJPEG.h"

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
    bool ImageCodecPlugin::_initializePlugin()
    {
        Helper::registerDecoder<ImageDecoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        Helper::registerDecoder<ImageDecoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        Helper::registerDecoder<ImageDecoderPVRTC>( STRINGIZE_STRING_LOCAL( "pvrImage" ) );
        Helper::registerDecoder<ImageDecoderETC1>( STRINGIZE_STRING_LOCAL( "etcImage" ) );
        Helper::registerDecoder<ImageDecoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        Helper::registerDecoder<ImageDecoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::registerDecoder<ImageDecoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "png", STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "PNG", STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "jpg", STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "jpeg", STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "pvr", STRINGIZE_STRING_LOCAL( "pvrImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "dds", STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "etc", STRINGIZE_STRING_LOCAL( "etcImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "htf", STRINGIZE_STRING_LOCAL( "htfImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "acf", STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::registerEncoder<ImageEncoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        Helper::registerEncoder<ImageEncoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ) );
        Helper::registerEncoder<ImageEncoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::registerEncoder<ImageEncoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::registerEncoder<ImageEncoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        Helper::registerDecoder<PickDecoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ) );
        Helper::registerEncoder<PickEncoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ) );

        CODEC_SERVICE()
            ->registerCodecExt( "hit", STRINGIZE_STRING_LOCAL( "hitPick" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageCodecPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "pvrImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "etcImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "acfImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "jpegImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "hitPick" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "hitPick" ) );
    }
}