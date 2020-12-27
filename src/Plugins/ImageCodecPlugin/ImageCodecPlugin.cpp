#include "ImageCodecPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderHTF.h"
#include "ImageEncoderHTF.h"
#include "ImageDecoderACF.h"
#include "ImageEncoderACF.h"

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
        Helper::registerDecoder<ImageDecoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "htf" ), STRINGIZE_STRING_LOCAL( "htfImage" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "acf" ), STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::registerEncoder<ImageEncoderHTF>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<ImageEncoderACF>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        Helper::registerDecoder<PickDecoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<PickEncoderHIT>( STRINGIZE_STRING_LOCAL( "hitPick" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "hit" ), STRINGIZE_STRING_LOCAL( "hitPick" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageCodecPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "htfImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "acfImage" ) );

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "hitPick" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "hitPick" ) );

        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "htf" ) );
        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "acf" ) );

        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "hit" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageCodecPlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}