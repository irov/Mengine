#include "TheoraPlugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#include "TheoraVideoDecoder.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Theora, Mengine::TheoraPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * ogg_malloc( size_t _size, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        void * p = ALLOCATOR_SERVICE()
            ->malloc( _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * ogg_calloc( size_t _num, size_t _size, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        void * p = ALLOCATOR_SERVICE()
            ->calloc( _num, _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * ogg_realloc( void * _ptr, size_t _size, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        void * p = ALLOCATOR_SERVICE()
            ->realloc( _ptr, _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void ogg_free( void * _ptr, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        ALLOCATOR_SERVICE()
            ->free( _ptr, "ogg" );
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraPlugin::TheoraPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraPlugin::~TheoraPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraPlugin::_initializePlugin()
    {
        OGGAllocatorEx allocator;
        allocator.ud = nullptr;
        allocator.malloc = &ogg_malloc;
        allocator.calloc = &ogg_calloc;
        allocator.realloc = &ogg_realloc;
        allocator.free = &ogg_free;

        setOGGAllocatorEx( &allocator );

        LOGGER_MESSAGE_RELEASE( "Video Theora Version: %s"
            , theora_version_string()
        );

        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvVideo" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<TheoraVideoDecoder>( STRINGIZE_STRING_LOCAL( "ogvaVideo" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogv" ), STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogva" ), STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvVideo" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvaVideo" ) );

        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogv" ) );
        CODEC_UNREGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogva" ) );
    }
}