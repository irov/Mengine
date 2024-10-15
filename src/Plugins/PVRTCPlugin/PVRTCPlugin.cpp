#include "PVRTCPlugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderPVRTC.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PVRTC, Mengine::PVRTCPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PVRTCPlugin::PVRTCPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PVRTCPlugin::~PVRTCPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PVRTCPlugin::_initializePlugin()
    {
        Helper::registerDecoder<ImageDecoderPVRTC>( STRINGIZE_STRING_LOCAL( "pvrImage" ), MENGINE_DOCUMENT_FACTORABLE );
        
        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "pvr" ), STRINGIZE_STRING_LOCAL( "pvrImage" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PVRTCPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "pvrImage" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "pvr" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void PVRTCPlugin::_destroyPlugin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}