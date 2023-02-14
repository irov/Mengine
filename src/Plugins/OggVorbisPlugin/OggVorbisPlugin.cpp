#include "OggVorbisPlugin.h"

#include "Interface/CodecServiceInterface.h"

#include "SoundDecoderOGGVorbis.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
void * _ogg_malloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "ogg" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void * _ogg_calloc( size_t _num, size_t _size )
{
    void * p = Mengine::Helper::callocateMemory( _num, _size, "ogg" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void * _ogg_realloc( void * _ptr, size_t _size )
{
    void * p = Mengine::Helper::reallocateMemory( _ptr, _size, "ogg" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _ogg_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "ogg" );
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OggVorbis, Mengine::OggVorbisPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
        const char * vorbis_version = vorbis_version_string();
        
        MENGINE_UNUSED( vorbis_version );

        LOGGER_INFO( "vorbis", "Vorbis version: %s"
            , vorbis_version
        );

        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "oggSound" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<SoundDecoderOGGVorbis>( STRINGIZE_STRING_LOCAL( "ogvSound" ), MENGINE_DOCUMENT_FACTORABLE );

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "ogg" ), STRINGIZE_STRING_LOCAL( "oggSound" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OggVorbisPlugin::_finalizePlugin()
    {
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "oggSound" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "ogvSound" ) );

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "ogg" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OggVorbisPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "ogg" );
    }
    //////////////////////////////////////////////////////////////////////////
}
