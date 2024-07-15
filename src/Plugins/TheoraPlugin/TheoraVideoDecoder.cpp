#include "TheoraVideoDecoder.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionType.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/Array.h"

#include "Config/StdString.h"

#ifndef MENGINE_THEORA_OGG_BUFFER_SIZE
#define MENGINE_THEORA_OGG_BUFFER_SIZE 4096
#endif

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
#   define THEORA_COLOR_R 0
#   define THEORA_COLOR_G 1
#   define THEORA_COLOR_B 2
#   define THEORA_COLOR_A 3
#else
#   define THEORA_COLOR_R 2
#   define THEORA_COLOR_G 1
#   define THEORA_COLOR_B 0
#   define THEORA_COLOR_A 3
#endif

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr int32_t createCoefYTableFunction( size_t index )
        {
            const int32_t scale = (1L << 13);

            float base = (1.164f * scale + 0.5f);

            int32_t value = (int32_t)(base * index - base * 16.f);

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t ... index>
        constexpr auto createCoefYTable( std::index_sequence<index ...> )
        {
            return Array<int32_t, sizeof ... (index)>{{Detail::createCoefYTableFunction( index ) ...}};
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t size>
        constexpr auto createCoefYTable()
        {
            return Detail::createCoefYTable( std::make_index_sequence<size>{} );
        }
        //////////////////////////////////////////////////////////////////////////
        constexpr int32_t createCoefUVTableFunction( float coeff, size_t index )
        {
            const int32_t scale = (1L << 13);

            float base = (coeff * scale + 0.5f);

            int32_t value = (int32_t)(base * index - base * 128.f);

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t ... index>
        constexpr auto createCoefUVTable( float coeff, std::index_sequence<index ...> )
        {
            return Array<int32_t, sizeof ... (index)>{{Detail::createCoefUVTableFunction( coeff, index ) ...}};
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t size>
        constexpr auto createCoefUVTable( float coeff )
        {
            return Detail::createCoefUVTable( coeff, std::make_index_sequence<size>{} );
        }
        //////////////////////////////////////////////////////////////////////////
        static const Array<int32_t, 256> YTable = Detail::createCoefYTable<256>();
        static const Array<int32_t, 256> RVTable = Detail::createCoefUVTable<256>( 1.596f );
        static const Array<int32_t, 256> GUTable = Detail::createCoefUVTable<256>( 0.391f );
        static const Array<int32_t, 256> GVTable = Detail::createCoefUVTable<256>( 0.813f );
        static const Array<int32_t, 256> BUTable = Detail::createCoefUVTable<256>( 2.018f );
        static const Char * pixelformats[] = {"OC_PF_420", "OC_PF_RSVD", "OC_PF_422", "OC_PF_444"};
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraVideoDecoder::TheoraVideoDecoder()
        : m_time( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraVideoDecoder::~TheoraVideoDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_initialize()
    {
        ogg_sync_init( &m_oggSyncState );

        theora_comment_init( &m_theoraComment );
        theora_info_init( &m_theoraInfo );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_prepareData()
    {
        bool stateFlag = true;
        bool theoraHeader = false;

        do
        {
            size_t bytes = this->readBufferData_();

            if( bytes == 0 )
            {
                LOGGER_ERROR( "theora video decoder bad file" );

                return false;
            }

            ogg_page page;
            while( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
            {
                if( ogg_page_bos( &page ) == false )
                {
                    if( ogg_stream_pagein( &m_oggStreamState, &page ) == -1 )
                    {
                        LOGGER_ERROR( "theora video decoder bad page in" );

                        return false;
                    }

                    stateFlag = false;
                    break;
                }

                ogg_stream_state oggStreamStateTest;
                MENGINE_MEMSET( &oggStreamStateTest, 0x00, sizeof( ogg_stream_state ) );

                int32_t serialno = ogg_page_serialno( &page );
                if( ogg_stream_init( &oggStreamStateTest, serialno ) != 0 )
                {
                    LOGGER_ERROR( "theora video decoder error during ogg_stream_init" );

                    return false;
                }

                if( ogg_stream_pagein( &oggStreamStateTest, &page ) != 0 )
                {
                    LOGGER_ERROR( "theora video decoder error during ogg_stream_pagein" );

                    return false;
                }

                ogg_packet packet;
                if( ogg_stream_packetout( &oggStreamStateTest, &packet ) == -1 )
                {
                    LOGGER_ERROR( "theora video decoder error during ogg_stream_packetout" );

                    return false;
                }

                if( theoraHeader == true || theora_decode_header( &m_theoraInfo, &m_theoraComment, &packet ) < 0 )
                {
                    ogg_stream_clear( &oggStreamStateTest );
                }
                else
                {
                    MENGINE_MEMCPY( &m_oggStreamState, &oggStreamStateTest, sizeof( ogg_stream_state ) );

                    theoraHeader = true;
                }
            }
        } while( stateFlag == true );

        if( theoraHeader == false )
        {
            return false;
        }

        int32_t theoraHeaderPackets = 1;

        while( theoraHeaderPackets < 3 )
        {
            while( theoraHeaderPackets < 3 )
            {
                ogg_packet packet;
                int32_t result = ogg_stream_packetout( &m_oggStreamState, &packet );

                if( result < 0 )
                {
                    LOGGER_ERROR( "theora video decoder error during ogg_stream_packetout %d"
                        , result
                    );

                    return false;
                }

                if( result == 0 )
                {
                    break;
                }

                int32_t result2 = theora_decode_header( &m_theoraInfo, &m_theoraComment, &packet );

                if( result2 < 0 )
                {
                    LOGGER_ERROR( "theora video decoder error during theora_decode_header (corrupt stream) %d"
                        , result2
                    );

                    return false;
                }

                ++theoraHeaderPackets;
            }

            ogg_page page;
            if( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
            {
                if( ogg_stream_pagein( &m_oggStreamState, &page ) == -1 )
                {
                    LOGGER_ERROR( "theora video decoder invalid page..." );

                    return false;
                }
            }
            else
            {
                size_t bytes = this->readBufferData_();

                if( bytes == 0 )
                {
                    LOGGER_ERROR( "theora video decoder eof searched. terminate..." );

                    return false;
                }
            }
        }

        if( theora_decode_init( &m_theoraState, &m_theoraInfo ) != 0 )
        {
            return false;
        }

        MENGINE_ASSERTION_FATAL( m_theoraInfo.pixelformat == OC_PF_420, "invalid support pixel format '%s' pls use OC_PF_420"
            , Detail::pixelformats[m_theoraInfo.pixelformat]
        );

        MENGINE_ASSERTION_FATAL( m_theoraInfo.fps_denominator != 0, "invalid fps denominator" );

        m_dataInfo.frameWidth = m_theoraInfo.frame_width;
        m_dataInfo.frameHeight = m_theoraInfo.frame_height;

        m_dataInfo.width = m_theoraInfo.width;
        m_dataInfo.height = m_theoraInfo.height;

        m_dataInfo.fps = (float)m_theoraInfo.fps_numerator / (float)m_theoraInfo.fps_denominator;

        m_dataInfo.format = PF_R8G8B8;
        m_dataInfo.clamp = true;

        m_time = 0.f;
        
        float time;
        if( this->readNextFrame( 0.f, &time ) == VDRS_FAILURE )
        {
            return 0;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraVideoDecoder::_finalize()
    {
        ogg_stream_clear( &m_oggStreamState );
        ogg_sync_clear( &m_oggSyncState );

        theora_clear( &m_theoraState );
        theora_comment_clear( &m_theoraComment );
        theora_info_clear( &m_theoraInfo );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_rewind()
    {
        ogg_stream_clear( &m_oggStreamState );
        ogg_sync_clear( &m_oggSyncState );

        theora_clear( &m_theoraState );
        theora_comment_clear( &m_theoraComment );
        theora_info_clear( &m_theoraInfo );

        MENGINE_MEMSET( &m_oggStreamState, 0, sizeof( m_oggStreamState ) );
        MENGINE_MEMSET( &m_oggSyncState, 0, sizeof( m_oggSyncState ) );
        MENGINE_MEMSET( &m_theoraState, 0, sizeof( m_theoraState ) );
        MENGINE_MEMSET( &m_theoraComment, 0, sizeof( m_theoraComment ) );
        MENGINE_MEMSET( &m_theoraInfo, 0, sizeof( m_theoraInfo ) );

        ogg_sync_init( &m_oggSyncState );

        theora_comment_init( &m_theoraComment );
        theora_info_init( &m_theoraInfo );

        const InputStreamInterfacePtr & stream = this->getStream();

        stream->rewind();

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t TheoraVideoDecoder::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData );
        MENGINE_ASSERTION_TYPE( _decoderData, const VideoDecoderData * );

        MENGINE_PROFILER_CATEGORY();

        const VideoDecoderData * decoderData = static_cast<const VideoDecoderData *>(_decoderData);

        yuv_buffer yuvBuffer;
        int32_t error_code = theora_decode_YUVout( &m_theoraState, &yuvBuffer );

        if( error_code < 0 )
        {
            LOGGER_ERROR( "theora video decoder error during theora_decode_YUVout '%d'"
                , error_code
            );

            return 0;
        }

        if( this->decodeBuffer_( yuvBuffer, decoderData ) == false )
        {
            return 0;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::decodeBuffer_( const yuv_buffer & _yuvBuffer, const VideoDecoderData * _decoderData )
    {
        #define THEORA_CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) rgb_char_buffer = (uint8_t)MENGINE_CLAMP( 0, rgb_color_test, 255)

        uint8_t * dstBitmap = static_cast<uint8_t *>(_decoderData->buffer);
        uint8_t * dstBitmapOffset = static_cast<uint8_t *>(_decoderData->buffer) + _decoderData->pitch;

        bool separateHorizontalAlpha = (_decoderData->flags & DF_VIDEO_SEPARATE_HORIZONTAL_ALPHA) == DF_VIDEO_SEPARATE_HORIZONTAL_ALPHA;
        bool premultiplyAlpha = (_decoderData->flags & DF_VIDEO_PREMULTIPLY_ALPHA) == DF_VIDEO_PREMULTIPLY_ALPHA;

        if( separateHorizontalAlpha == false && _decoderData->format == PF_X8R8G8B8 )
        {
            size_t dstOff = _decoderData->pitch * 2 - m_theoraInfo.width * 4;
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t *)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t *)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t *)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            for( int32_t y = 0; y != y_height; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    int32_t rV = Detail::RVTable[v];
                    int32_t gUV = Detail::GUTable[u] + Detail::GVTable[v];
                    int32_t bU = Detail::BUTable[u];

                    int32_t rgbY = Detail::YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[0 + THEORA_COLOR_B] );
                    dstBitmap[THEORA_COLOR_A] = 255;
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[4 + THEORA_COLOR_B] );
                    dstBitmap[4 + THEORA_COLOR_A] = 255;
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[0 + THEORA_COLOR_B] );
                    dstBitmapOffset[THEORA_COLOR_A] = 255;
                    ++ySrc2;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[4 + THEORA_COLOR_B] );
                    dstBitmapOffset[4 + THEORA_COLOR_A] = 255;
                    ++ySrc2;

                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                }

                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            }
        }
        else if( separateHorizontalAlpha == false && _decoderData->format == PF_R8G8B8 )
        {
            size_t dstOff = _decoderData->pitch * 2 - m_theoraInfo.width * 3;
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t *)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t *)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t *)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            for( int32_t y = 0; y != y_height; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    int32_t rV = Detail::RVTable[v];
                    int32_t gUV = Detail::GUTable[u] + Detail::GVTable[v];
                    int32_t bU = Detail::BUTable[u];

                    int32_t rgbY = Detail::YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[0 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[3 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[3 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[3 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[0 + THEORA_COLOR_B] );
                    ++ySrc2;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[3 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[3 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[3 + THEORA_COLOR_B] );
                    ++ySrc2;

                    dstBitmap += 3 << 1;
                    dstBitmapOffset += 3 << 1;
                }

                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            }
        }
        else if( separateHorizontalAlpha == true && _decoderData->format == PF_A8R8G8B8 )
        {
            size_t dstOff = _decoderData->pitch * 2 - m_theoraInfo.width * 4;
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t *)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t *)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t *)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            int32_t y_rgb_count = y_height / 2;

            for( int32_t y = 0; y != y_rgb_count; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    int32_t rV = Detail::RVTable[v];
                    int32_t gUV = Detail::GUTable[u] + Detail::GVTable[v];
                    int32_t bU = Detail::BUTable[u];

                    int32_t rgbY = Detail::YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[0 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[4 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[0 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[0 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[0 + THEORA_COLOR_B] );
                    ++ySrc2;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[4 + THEORA_COLOR_B] );
                    ++ySrc2;

                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                }

                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            }

            dstBitmap = static_cast<uint8_t *>(_decoderData->buffer);
            dstBitmapOffset = static_cast<uint8_t *>(_decoderData->buffer) + _decoderData->pitch;

            for( int32_t y = 0; y != y_rgb_count; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    int32_t v = vSrc[x];

                    int32_t rV = Detail::RVTable[v];

                    int32_t rgbY = Detail::YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[0 + THEORA_COLOR_A] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_A] );
                    ++ySrc;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[0 + THEORA_COLOR_A] );
                    ++ySrc2;

                    rgbY = Detail::YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[4 + THEORA_COLOR_A] );
                    ++ySrc2;

                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                }

                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                vSrc += _yuvBuffer.uv_stride;
            }

            if( premultiplyAlpha == true )
            {
                dstBitmap = static_cast<uint8_t *>(_decoderData->buffer);
                dstBitmapOffset = static_cast<uint8_t *>(_decoderData->buffer) + _decoderData->pitch;

                for( int32_t y = 0; y != y_rgb_count; ++y )
                {
                    for( int32_t x = 0; x != y_width; ++x )
                    {
                        dstBitmap[0 + THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmap[0 + THEORA_COLOR_R] * dstBitmap[0 + THEORA_COLOR_A] / 255);
                        dstBitmap[0 + THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmap[0 + THEORA_COLOR_G] * dstBitmap[0 + THEORA_COLOR_A] / 255);
                        dstBitmap[0 + THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmap[0 + THEORA_COLOR_B] * dstBitmap[0 + THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_R] * dstBitmap[4 + THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_G] * dstBitmap[4 + THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_B] * dstBitmap[4 + THEORA_COLOR_A] / 255);

                        dstBitmapOffset[0 + THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmapOffset[0 + THEORA_COLOR_R] * dstBitmapOffset[0 + THEORA_COLOR_A] / 255);
                        dstBitmapOffset[0 + THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmapOffset[0 + THEORA_COLOR_G] * dstBitmapOffset[0 + THEORA_COLOR_A] / 255);
                        dstBitmapOffset[0 + THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmapOffset[0 + THEORA_COLOR_B] * dstBitmapOffset[0 + THEORA_COLOR_A] / 255);
                        dstBitmapOffset[4 + THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmapOffset[4 + THEORA_COLOR_R] * dstBitmapOffset[4 + THEORA_COLOR_A] / 255);
                        dstBitmapOffset[4 + THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmapOffset[4 + THEORA_COLOR_G] * dstBitmapOffset[4 + THEORA_COLOR_A] / 255);
                        dstBitmapOffset[4 + THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmapOffset[4 + THEORA_COLOR_B] * dstBitmapOffset[4 + THEORA_COLOR_A] / 255);

                        dstBitmap += 4 << 1;
                        dstBitmapOffset += 4 << 1;
                    }

                    dstBitmap += dstOff;
                    dstBitmapOffset += dstOff;
                }
            }
        }
        else
        {
            LOGGER_ERROR( "theora video decoder unsupport format" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t TheoraVideoDecoder::readBufferData_()
    {
        char * buffer = ogg_sync_buffer( &m_oggSyncState, MENGINE_THEORA_OGG_BUFFER_SIZE );

        if( buffer == nullptr )
        {
            return 0;
        }

        const InputStreamInterfacePtr & stream = this->getStream();

        size_t bytes = stream->read( buffer, MENGINE_THEORA_OGG_BUFFER_SIZE );

        long ogg_bytes = (long)bytes;

        if( ogg_sync_wrote( &m_oggSyncState, ogg_bytes ) == -1 )
        {
            return 0;
        }

        return bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::isAlphaSplitVideo() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TheoraVideoDecoder::getSurfaceDimension( uint32_t _flags, VideoSurfaceDimension * const _surfaceDimension ) const
    {
        bool separateHorizontAlpha = (_flags & DF_VIDEO_SEPARATE_HORIZONTAL_ALPHA) == DF_VIDEO_SEPARATE_HORIZONTAL_ALPHA;

        if( separateHorizontAlpha == true )
        {
            _surfaceDimension->frameWidth = m_dataInfo.frameWidth;
            _surfaceDimension->frameHeight = m_dataInfo.frameHeight / 2;

            _surfaceDimension->width = m_dataInfo.width;
            _surfaceDimension->height = m_dataInfo.height / 2;

            _surfaceDimension->format = PF_A8R8G8B8;
        }
        else
        {
            _surfaceDimension->frameWidth = m_dataInfo.frameWidth;
            _surfaceDimension->frameHeight = m_dataInfo.frameHeight;

            _surfaceDimension->width = m_dataInfo.width;
            _surfaceDimension->height = m_dataInfo.height;

            _surfaceDimension->format = PF_R8G8B8;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    EVideoDecoderReadState TheoraVideoDecoder::readNextFrame( float _request, float * const _pts )
    {
        MENGINE_UNUSED( _request );
        MENGINE_UNUSED( _pts );

        EVideoDecoderReadState state = VDRS_SUCCESS;

        ogg_packet packet;

        for( ;; )
        {
            int32_t error_packetout = ogg_stream_packetout( &m_oggStreamState, &packet );

            if( error_packetout > 0 )
            {
                break;
            }
            else if( error_packetout < 0 )
            {
                return VDRS_FAILURE;
            }

            size_t bytes = this->readBufferData_();

            if( bytes == 0 )
            {
                return VDRS_END_STREAM;
            }

            ogg_page page;
            while( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
            {
                ogg_stream_pagein( &m_oggStreamState, &page );
            }
        }

        if( theora_decode_packetin( &m_theoraState, &packet ) == OC_BADPACKET )
        {
            return VDRS_FAILURE;
        }

        double time = theora_granule_time( &m_theoraState, m_theoraState.granulepos );

        m_time = (float)(time * 1000.0);

        *_pts = m_time;

        return state;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::seekToFrame( float _time )
    {
        float frameTiming = 1000.f / m_dataInfo.fps;

        for( ;; )
        {
            ogg_packet packet;

            for( ;; )
            {
                int32_t error_packetout = ogg_stream_packetout( &m_oggStreamState, &packet );

                if( error_packetout > 0 )
                {
                    break;
                }
                else if( error_packetout < 0 )
                {
                    return false;
                }

                size_t bytes = this->readBufferData_();

                if( bytes == 0 )
                {
                    return false;
                }

                ogg_page page;
                while( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
                {
                    ogg_stream_pagein( &m_oggStreamState, &page );
                }
            }

            if( theora_decode_packetin( &m_theoraState, &packet ) == OC_BADPACKET )
            {
                return false;
            }

            double time = theora_granule_time( &m_theoraState, m_theoraState.granulepos );

            float pts = (float)(time * 1000.0);

            if( pts + frameTiming < _time )
            {
                continue;
            }

            m_time = pts;

            break;
        }       

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_seek( float _time )
    {
        float frameTime = 1000.f / m_dataInfo.fps;

        uint32_t frame_time = (uint32_t)(m_time / frameTime);
        uint32_t frame_seek = (uint32_t)(_time / frameTime);

        if( frame_time == frame_seek || frame_time == frame_seek + 1 )
        {
            return true;
        }

        if( frame_time > frame_seek )
        {
            if( this->_rewind() == false )
            {
                return false;
            }

            if( frame_seek == 0 )
            {
                return true;
            }
        }

        //bool successful = this->seekToFrame( _time );
        //return successful;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float TheoraVideoDecoder::_tell() const
    {
        double granule_time = theora_granule_time( &m_theoraState, m_theoraState.granulepos );

        float timing = (float)(granule_time * 1000.0);

        return timing;
    }
    //////////////////////////////////////////////////////////////////////////
}
