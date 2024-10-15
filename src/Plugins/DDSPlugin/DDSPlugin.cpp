#include "DDSPlugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderDDS.h"
#include "ImageEncoderDDS.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DDS, Mengine::DDSPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DDSPlugin::DDSPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DDSPlugin::~DDSPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DDSPlugin::_initializePlugin()
    {
        Helper::registerDecoder<ImageDecoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "dds" ), STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        Helper::registerEncoder<ImageEncoderDDS>( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DDSPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ddsImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "ddsImage" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "dds" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DDSPlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}