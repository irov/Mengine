#include "OggVorbisPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderOGGVorbis.h"

#include "Kernel/AllocatorHelper.h"
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

        void * p = Helper::allocateMemory( _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * ogg_calloc( size_t _num, size_t _size, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        void * p = Helper::callocateMemory( _num, _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * ogg_realloc( void * _ptr, size_t _size, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        void * p = Helper::reallocateMemory( _ptr, _size, "ogg" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void ogg_free( void * _ptr, void * _ud )
    {
        MENGINE_UNUSED( _ud );

        Helper::deallocateMemory( _ptr, "ogg" );
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