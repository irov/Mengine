#include "JPEGPlugin.h"

#include "Interface/CodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ImageCodecInterface.h"

#include "ImageDecoderJPEG.h"
#include "ImageEncoderJPEG.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/EncoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * _jpeg_malloc( size_t _size )
    {
        void * p = Mengine::Helper::allocateMemory( _size, "jpeg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void _jpeg_free( void * _ptr )
    {
        Mengine::Helper::deallocateMemory( _ptr, "jpeg" );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( JPEG, Mengine::JPEGPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JPEGPlugin::JPEGPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JPEGPlugin::~JPEGPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JPEGPlugin::_initializePlugin()
    {
        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "jpg" ), STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "jpeg" ), STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        Helper::registerDecoder<ImageDecoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerEncoder<ImageEncoderJPEG>( STRINGIZE_STRING_LOCAL( "jpegImage" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JPEGPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "jpegImage" ) );
        Helper::unregisterEncoder( STRINGIZE_STRING_LOCAL( "jpegImage" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "jpg" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "jpeg" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void JPEGPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "jpeg" );
    }
    //////////////////////////////////////////////////////////////////////////
}