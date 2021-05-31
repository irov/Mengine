#include "TheoraVideoDecoder.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionType.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_THEORA_OGG_BUFFER_SIZE
#define MENGINE_THEORA_OGG_BUFFER_SIZE 4096
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_RENDER_TEXTURE_RGBA
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
//////////////////////////////////////////////////////////////////////////
#define THEORA_CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) \
    rgb_char_buffer = (uint8_t)MENGINE_CLAMP( 0, rgb_color_test, 255)
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static int32_t s_YTable[256];
    static int32_t s_BUTable[256];
    static int32_t s_GUTable[256];
    static int32_t s_GVTable[256];
    static int32_t s_RVTable[256];
    //////////////////////////////////////////////////////////////////////////
    static void s_createCoefTables()
    {
        int32_t scale = 1L << 13;

        for( int32_t i = 0; i != 256; ++i )
        {
            int32_t temp = i - 128;

            s_YTable[i] = (int32_t)((1.164f * scale + 0.5f) * (i - 16));

            s_RVTable[i] = (int32_t)((1.596f * scale + 0.5f) * temp);

            s_GUTable[i] = (int32_t)((0.391f * scale + 0.5f) * temp);
            s_GVTable[i] = (int32_t)((0.813f * scale + 0.5f) * temp);

            s_BUTable[i] = (int32_t)((2.018f * scale + 0.5f) * temp);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraVideoDecoder::TheoraVideoDecoder()
        : m_time( 0.f )
        , m_readyFrame( false )
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

        static bool s_initializeCoefTables = false;
        if( s_initializeCoefTables == false )
        {
            s_initializeCoefTables = true;
            s_createCoefTables();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_prepareData()
    {
        bool stateFlag = true;
        bool theoraHeader = false;

        do
        {
            if( this->read_buffer_data_() == 0 )
            {
                LOGGER_ERROR( "bad file" );

                return false;
            }

            ogg_page page;
            while( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
            {
                if( ogg_page_bos( &page ) == false )
                {
                    if( ogg_stream_pagein( &m_oggStreamState, &page ) == -1 )
                    {
                        LOGGER_ERROR( "bad page in" );

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
                    LOGGER_ERROR( "error during ogg_stream_init" );

                    return false;
                }

                if( ogg_stream_pagein( &oggStreamStateTest, &page ) != 0 )
                {
                    LOGGER_ERROR( "error during ogg_stream_pagein" );

                    return false;
                }

                ogg_packet packet;
                if( ogg_stream_packetout( &oggStreamStateTest, &packet ) == -1 )
                {
                    LOGGER_ERROR( "error during ogg_stream_packetout" );

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
                    LOGGER_ERROR( "error during ogg_stream_packetout %d"
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
                    LOGGER_ERROR( "error during theora_decode_header (corrupt stream) %d"
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
                    LOGGER_ERROR( "invalid page..." );

                    return false;
                }
            }
            else
            {
                size_t ret = this->read_buffer_data_();

                if( ret == 0 )
                {
                    LOGGER_ERROR( "eof searched. terminate..." );

                    return false;
                }
            }
        }

        theora_decode_init( &m_theoraState, &m_theoraInfo );

        if( m_theoraInfo.pixelformat != OC_PF_420 )
        {
            const Char * pixelformat[] = {"OC_PF_420", "OC_PF_RSVD", "OC_PF_422", "OC_PF_444"};

            LOGGER_ERROR( "invalid support pixel format '%s' pls use OC_PF_420"
                , pixelformat[m_theoraInfo.pixelformat]
            );

            return false;
        }

        if( m_options.alpha == true )
        {
            m_dataInfo.frameWidth = m_theoraInfo.frame_width;
            m_dataInfo.frameHeight = m_theoraInfo.frame_height / 2;

            m_dataInfo.width = m_theoraInfo.width;
            m_dataInfo.height = m_theoraInfo.height / 2;

            m_dataInfo.format = PF_A8R8G8B8;
        }
        else
        {
            m_dataInfo.frameWidth = m_theoraInfo.frame_width;
            m_dataInfo.frameHeight = m_theoraInfo.frame_height;

            m_dataInfo.width = m_theoraInfo.width;
            m_dataInfo.height = m_theoraInfo.height;

            m_dataInfo.format = PF_R8G8B8;
        }

        m_dataInfo.duration = m_options.duration;
        m_dataInfo.fps = m_options.fps;

        m_time = 0.f;
        m_readyFrame = false;

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

        if( m_stream->seek( 0 ) == false )
        {
            return false;
        }

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t TheoraVideoDecoder::_decode( const DecoderData * _data )
    {
        MENGINE_ASSERTION_TYPE( _data, const VideoDecoderData * );

        if( m_readyFrame == false )
        {
            float time;
            if( this->readNextFrame( 0.f, &time ) == VDRS_FAILURE )
            {
                return 0;
            }

            m_readyFrame = true;
        }

        yuv_buffer yuvBuffer;
        int32_t error_code = theora_decode_YUVout( &m_theoraState, &yuvBuffer );

        if( error_code < 0 )
        {
            LOGGER_ERROR( "error during theora_decode_YUVout '%d'"
                , error_code
            );

            return 0;
        }

        if( this->decodeBuffer_( yuvBuffer, static_cast<const VideoDecoderData *>(_data) ) == false )
        {
            return 0;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::decodeBuffer_( const yuv_buffer & _yuvBuffer, const VideoDecoderData * _data )
    {
        uint8_t * dstBitmap = static_cast<uint8_t *>(_data->buffer);
        uint8_t * dstBitmapOffset = static_cast<uint8_t *>(_data->buffer) + _data->pitch;

        if( m_options.alpha == false && _data->format == PF_X8R8G8B8 )
        {
            uint32_t dstOff = _data->pitch * 2 - m_theoraInfo.width * 4;
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

                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    dstBitmap[THEORA_COLOR_A] = 255;
                    ++ySrc;

                    rgbY = s_YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[4 + THEORA_COLOR_B] );
                    dstBitmap[4 + THEORA_COLOR_A] = 255;
                    ++ySrc;

                    rgbY = s_YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[THEORA_COLOR_B] );
                    dstBitmapOffset[THEORA_COLOR_A] = 255;
                    ++ySrc2;

                    rgbY = s_YTable[*ySrc2];
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
        else if( m_options.alpha == false && _data->format == PF_R8G8B8 )
        {
            uint32_t dstOff = _data->pitch * 2 - m_theoraInfo.width * 3;
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

                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[3 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[3 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[3 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[THEORA_COLOR_B] );
                    ++ySrc2;

                    rgbY = s_YTable[*ySrc2];
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
        else if( m_options.alpha == true && _data->format == PF_A8R8G8B8 )
        {
            uint32_t dstOff = _data->pitch * 2 - m_theoraInfo.width * 4;
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

                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[4 + THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[4 + THEORA_COLOR_B] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    g = (rgbY - gUV) >> 13;
                    b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmapOffset[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmapOffset[THEORA_COLOR_B] );
                    ++ySrc2;

                    rgbY = s_YTable[*ySrc2];
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

            dstBitmap = static_cast<uint8_t *>(_data->buffer);
            dstBitmapOffset = static_cast<uint8_t *>(_data->buffer) + _data->pitch;

            for( int32_t y = 0; y != y_rgb_count; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    int32_t v = vSrc[x];

                    int32_t rV = s_RVTable[v];

                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_A] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc];
                    r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[4 + THEORA_COLOR_A] );
                    ++ySrc;

                    rgbY = s_YTable[*ySrc2];
                    r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmapOffset[THEORA_COLOR_A] );
                    ++ySrc2;

                    rgbY = s_YTable[*ySrc2];
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

            if( m_options.premultiply == true )
            {
                dstBitmap = static_cast<uint8_t *>(_data->buffer);
                dstBitmapOffset = static_cast<uint8_t *>(_data->buffer) + _data->pitch;

                for( int32_t y = 0; y != y_rgb_count; ++y )
                {
                    for( int32_t x = 0; x != y_width; ++x )
                    {
                        dstBitmap[THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmap[THEORA_COLOR_R] * dstBitmap[THEORA_COLOR_A] / 255);
                        dstBitmap[THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmap[THEORA_COLOR_G] * dstBitmap[THEORA_COLOR_A] / 255);
                        dstBitmap[THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmap[THEORA_COLOR_B] * dstBitmap[THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_R] * dstBitmap[4 + THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_G] * dstBitmap[4 + THEORA_COLOR_A] / 255);
                        dstBitmap[4 + THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmap[4 + THEORA_COLOR_B] * dstBitmap[4 + THEORA_COLOR_A] / 255);

                        dstBitmapOffset[THEORA_COLOR_R] = (uint8_t)((uint32_t)dstBitmapOffset[THEORA_COLOR_R] * dstBitmapOffset[THEORA_COLOR_A] / 255);
                        dstBitmapOffset[THEORA_COLOR_G] = (uint8_t)((uint32_t)dstBitmapOffset[THEORA_COLOR_G] * dstBitmapOffset[THEORA_COLOR_A] / 255);
                        dstBitmapOffset[THEORA_COLOR_B] = (uint8_t)((uint32_t)dstBitmapOffset[THEORA_COLOR_B] * dstBitmapOffset[THEORA_COLOR_A] / 255);
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
            LOGGER_ERROR( "unsupport format" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t TheoraVideoDecoder::read_buffer_data_()
    {
        char * buffer = ogg_sync_buffer( &m_oggSyncState, MENGINE_THEORA_OGG_BUFFER_SIZE );

        if( buffer == nullptr )
        {
            return 0;
        }

        size_t bytes = m_stream->read( buffer, MENGINE_THEORA_OGG_BUFFER_SIZE );

        long ogg_bytes = (long)bytes;

        if( ogg_sync_wrote( &m_oggSyncState, ogg_bytes ) == -1 )
        {
            return 0;
        }

        return bytes;
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

            size_t bytes = this->read_buffer_data_();

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
        float frameTiming = m_dataInfo.getFrameTime();

        ogg_packet packet;

        for( ;; )
        {
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

                size_t bytes = this->read_buffer_data_();

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

            double theora_time = theora_granule_time( &m_theoraState, packet.granulepos );

            float time = (float)(theora_time * 1000.0);

            if( time + frameTiming < _time )
            {
                continue;
            }

            m_time = time;

            break;
        }

        if( theora_decode_packetin( &m_theoraState, &packet ) == OC_BADPACKET )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::_seek( float _time )
    {
        float frameTime = m_dataInfo.getFrameTime();

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
