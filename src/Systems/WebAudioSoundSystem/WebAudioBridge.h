#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EWebAudioSourceState
    {
        EWASS_STOPPED = 0,
        EWASS_PLAYING = 1,
        EWASS_PAUSED = 2,
        EWASS_FINISHED = 3,
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    bool mengineAudioInitialize();
    void mengineAudioFinalize();
    uint32_t mengineAudioBufferCreate( const void * _pcm, uint32_t _frames, uint32_t _channels, uint32_t _frequency );
    uint32_t mengineAudioStreamCreate( const void * _data, size_t _size, float _duration );
    void mengineAudioBufferRelease( uint32_t _buffer );
    uint32_t mengineAudioSourceCreate();
    void mengineAudioSourceRelease( uint32_t _source );
    void mengineAudioSourceBuffer( uint32_t _source, uint32_t _buffer );
    bool mengineAudioPlay( uint32_t _source );
    void mengineAudioStop( uint32_t _source );
    void mengineAudioPause( uint32_t _source );
    bool mengineAudioResume( uint32_t _source );
    int32_t mengineAudioState( uint32_t _source );
    void mengineAudioVolume( uint32_t _source, float _volume );
    void mengineAudioLoop( uint32_t _source, bool _loop );
    bool mengineAudioSeek( uint32_t _source, float _position );
    float mengineAudioPosition( uint32_t _source );
}
//////////////////////////////////////////////////////////////////////////
