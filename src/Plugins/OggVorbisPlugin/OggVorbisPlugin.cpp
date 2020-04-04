#include "OggVorbisPlugin.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#include "SoundDecoderOGGVorbis.h"

#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OggVorbis, Mengine::OggVorbisPlugin );
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
    OggVorbisPlugin::OggVorbisPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OggVorbisPlugin::~OggVorbisPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OggVorbisPlugin::_initializePlugin()
    {
        OGGAllocatorEx allocator;
        allocator.ud = nullptr;
        allocator.malloc = &ogg_malloc;
        allocator.calloc = &ogg_calloc;
        allocator.realloc = &ogg_realloc;
        allocator.free = &ogg_free;

        setOGGAllocatorEx( &allocator );

        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "oggSound" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "ogvSound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_REGISTER_EXT( STRINGIZE_STRING_LOCAL( "ogg" ), STRINGIZE_STRING_LOCAL( "oggSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OggVorbisPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "oggSound" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvSound" ) );
    }
}