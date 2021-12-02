#include "PNGPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderPNG.h"
#include "ImageEncoderPNG.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PNG, Mengine::PNGPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PNGPlugin::PNGPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PNGPlugin::~PNGPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PNGPlugin::_initializePlugin()
    {
        LOGGER_MESSAGE( "PNG version: %s", PNG_HEADER_VERSION_STRING );

        Helper::registerDecoder<ImageDecoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "png" ), STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "PNG" ), STRINGIZE_STRING_LOCAL( "pngImage" ) );

        Helper::registerEncoder<ImageEncoderPNG>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PNGPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "pngImage" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "png" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "PNG" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PNGPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "dpng" );
        MENGINE_ASSERTION_ALLOCATOR( "epng" );
    }
    //////////////////////////////////////////////////////////////////////////
}