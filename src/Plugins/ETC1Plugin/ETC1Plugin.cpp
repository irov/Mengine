#include "ETC1Plugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderETC1.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ETC1, Mengine::ETC1Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ETC1Plugin::ETC1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ETC1Plugin::~ETC1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ETC1Plugin::_initializePlugin()
    {
        Helper::registerDecoder<ImageDecoderETC1>( STRINGIZE_STRING_LOCAL( "etcImage" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "etc" ), STRINGIZE_STRING_LOCAL( "etcImage" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ETC1Plugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "etcImage" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "etc" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ETC1Plugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}