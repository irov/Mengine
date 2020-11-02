#include "ImageCodecPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

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
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

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
        Helper::registerDecoder<ImageDecoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerDecoder<ImageDecoderPVRTC>( STRINGIZE_STRING_LOCAL( "pvrImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderETC1>( STRINGIZE_STRING_LOCAL( "etcImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerDecoder<ImageDecoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "png" ), STRINGIZE_STRING_LOCAL( "pngImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "PNG" ), STRINGIZE_STRING_LOCAL( "pngImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "jpg" ), STRINGIZE_STRING_LOCAL( "jpegImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "jpeg" ), STRINGIZE_STRING_LOCAL( "jpegImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "pvr" ), STRINGIZE_STRING_LOCAL( "pvrImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "dds" ), STRINGIZE_STRING_LOCAL( "ddsImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "etc" ), STRINGIZE_STRING_LOCAL( "etcImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "htf" ), STRINGIZE_STRING_LOCAL( "htfImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "acf" ), STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::registerEncoder<ImageEncoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<ImageEncoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<ImageEncoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<ImageEncoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerEncoder<ImageEncoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerDecoder<PickDecoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<PickEncoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "hit" ), STRINGIZE_STRING_LOCAL( "hitPick" ) );

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
    //////////////////////////////////////////////////////////////////////////
    void ImageCodecPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "jpeg" );
        MENGINE_ASSERTION_ALLOCATOR( "dpng" );
        MENGINE_ASSERTION_ALLOCATOR( "epng" );
    }
    //////////////////////////////////////////////////////////////////////////
}