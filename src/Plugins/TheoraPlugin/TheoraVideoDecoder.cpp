#include "TheoraVideoDecoder.h"

#include "Kernel/Logger.h"

#include <string.h>

#ifndef MENGINE_THEORA_OGG_BUFFER_SIZE
#define MENGINE_THEORA_OGG_BUFFER_SIZE 4096
#endif

//Defines
#define THEORA_MAX( a, b ) ((a > b) ? a : b)
#define THEORA_MIN( a, b ) ((a < b) ? a : b)

#ifdef MENGINE_RENDER_TEXTURE_RGBA
#	define THEORA_COLOR_R 0
#	define THEORA_COLOR_G 1
#	define THEORA_COLOR_B 2
#	define THEORA_COLOR_A 3
#else
#	define THEORA_COLOR_R 2
#	define THEORA_COLOR_G 1
#	define THEORA_COLOR_B 0
#	define THEORA_COLOR_A 3
#endif

#define THEORA_CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) \
	rgb_char_buffer = (uint8_t)(THEORA_MAX( THEORA_MIN(rgb_color_test, 255), 0 ))

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
        //used to bring the table into the high side (scale up) so we
        //can maintain high precision and not use floats (FIXED POINT)
        int32_t scale = 1L << 13;

        for( int32_t i = 0; i < 256; i++ )
        {
            int32_t temp = i - 128;

            s_YTable[i] = (int32_t)((1.164 * scale + 0.5) * (i - 16));	//Calc Y component

            s_RVTable[i] = (int32_t)((1.596 * scale + 0.5) * temp);		//Calc R component

            s_GUTable[i] = (int32_t)((0.391 * scale + 0.5) * temp);		//Calc G u & v components
            s_GVTable[i] = (int32_t)((0.813 * scale + 0.5) * temp);

            s_BUTable[i] = (int32_t)((2.018 * scale + 0.5) * temp);		//Calc B component
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TheoraVideoDecoder::TheoraVideoDecoder()
        : m_pitch( 0 )
        , m_time( 0.f )
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
        memset( &m_oggStreamState, 0, sizeof( m_oggStreamState ) );
        memset( &m_oggSyncState, 0, sizeof( m_oggSyncState ) );
        memset( &m_theoraState, 0, sizeof( m_theoraState ) );
        memset( &m_theoraComment, 0, sizeof( m_theoraComment ) );
        memset( &m_theoraInfo, 0, sizeof( m_theoraInfo ) );

        // start up Ogg stream synchronization layer
        ogg_sync_init( &m_oggSyncState );

        // init supporting Theora structures needed in header parsing
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
        // Ogg file open; parse the headers
        // Only interested in Vorbis/Theora streams 
        bool stateFlag = true;
        bool theoraHeader = false;  // число обработанных пакетов заголовков theora

        do
        {
            // и передает их в буфер приема ogg
            if( this->read_buffer_data_() == 0 )
            {
                // кончился файл, на данном этапе это ошибка
                LOGGER_ERROR( "Theora Codec Error: bad file" );

                return false;
            }

            // ogg_sync_pageout - формирует страницу
            ogg_page page;
            while( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
                // 1-больше данных не требуется
                // 0-требуется больше данных для создания страницы
            {

                // что страница сформирована успешно

                // это страница заголовков? если нет, кончаем искать заголовки
                if( ogg_page_bos( &page ) == false )
                {
                    // нет, это не страница заголовков
                    // значит, страницы заголовков всех логических потоков кончились
                    // и начались данные этих потоков
                    // таким образом надо переходить к чтению страниц данных

                    // закидываем эту страничку в логический видеопоток
                    if( ogg_stream_pagein( &m_oggStreamState, &page ) == -1 )
                    {
                        LOGGER_ERROR( "Theora Codec Error: bad page in" );

                        return false;
                    }
                    // закидывает страничку
                    // в логический поток theora, если
                    // совпадает идентификатор логического потока
                    // в противном случае страница игнорируется

                    // выходим из циклов
                    stateFlag = false;
                    break;
                }

                // да, это страница заголовков
                ogg_stream_state oggStreamStateTest;
                memset( &oggStreamStateTest, 0x00, sizeof( ogg_stream_state ) );

                // тестовый логический поток
                // инициализируем тестовый поток на тот же поток с таким же
                // идентификатором потока, как и у текущей странички
                int32_t serialno = ogg_page_serialno( &page );
                if( ogg_stream_init( &oggStreamStateTest, serialno ) != 0 )
                {
                    LOGGER_ERROR( "TheoraCodec Error: error during ogg_stream_init" );

                    return false;
                }

                // добавляем страницу в тестовый поток
                if( ogg_stream_pagein( &oggStreamStateTest, &page ) != 0 )
                {
                    LOGGER_ERROR( "TheoraCodec Error: error during ogg_stream_pagein" );

                    return false;
                }

                // декодируем данные из этого тестового потока в пакет
                ogg_packet packet;
                if( ogg_stream_packetout( &oggStreamStateTest, &packet ) == -1 )
                {
                    LOGGER_ERROR( "TheoraCodec Error: error during ogg_stream_packetout" );

                    return false;
                }

                // theoraHeaderPackets - число прочитанных
                // заголовочных ПАКЕТОВ theora (не страниц)
                // по спецификации theora таких пакетов должно быть три
                // декодируем заголовок theora
                if( theoraHeader == true || theora_decode_header( &m_theoraInfo, &m_theoraComment, &packet ) < 0 )
                {
                    // это не заголовок theora

                    // очищаем структуру тестового потока
                    ogg_stream_clear( &oggStreamStateTest );
                    //и продолжаем цикл в поисках заголовков theora
                }
                else
                {
                    // это заголовок theora!

                    // вот таким образом "инициализируем" логический поток theora:
                    // теперь из этого потока будут всегда сыпаться пакеты theora

                    memcpy( &m_oggStreamState, &oggStreamStateTest, sizeof( ogg_stream_state ) );

                    theoraHeader = true;

                    // после того, как мы нашли заголовочную страницу логического потока theora,
                    // нам необходимо прочитать все остальные заголовочные страницы
                    // других потоков и отбросить их (если таковые, конечно, имеются)
                }
            }
        } while( stateFlag == true );

        // сейчас надо получить еще два пакета заголовков theora (см. её документацию)
        // и можно переходить к потоковому воспроизведению

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
                // если функция возвращает нуль, значит не хватает данных для декодирования

                if( result < 0 )
                {
                    // ошибка декодирования, поврежденный поток
                    LOGGER_ERROR( "TheoraCodec Error: error during ogg_stream_packetout %d"
                        , result
                    );

                    return false;
                }

                if( result == 0 )
                {
                    break;
                }

                // удалось успешно извлечь пакет информации theora
                int32_t result2 = theora_decode_header( &m_theoraInfo, &m_theoraComment, &packet );

                if( result2 < 0 )
                {
                    // ошибка декодирования, поврежденный поток
                    LOGGER_ERROR( "TheoraCodec Error: error during theora_decode_header (corrupt stream) %d"
                        , result2
                    );

                    return false;
                }

                ++theoraHeaderPackets;
            }

            // эту страничку обработали, надо извлечь новую
            // для этого проверяем буфер чтения, вдруг там осталось что-нить похожее
            // на страничку. Если не осталось, тогда просто читаем эти данные из файла:			
            ogg_page page;
            if( ogg_sync_pageout( &m_oggSyncState, &page ) > 0 )
                // ogg_sync_pageout - функция, берет данные из буфера приема ogg
                // и записывает их в ogg_page
            {
                //мы нашли страничку в буфере и...
                if( ogg_stream_pagein( &m_oggStreamState, &page ) == -1 )
                {
                    // опять файл кончился!
                    LOGGER_ERROR( "TheoraCodec Error: invalid page..."
                    );

                    return false;
                }
                // ...пихаем эти данные в подходящий поток
            }
            else
            {
                // ничего мы в буфере не нашли
                size_t ret = this->read_buffer_data_();
                // надо больше данных! читаем их из файла
                if( ret == 0 )
                {
                    // опять файл кончился!
                    LOGGER_ERROR( "TheoraCodec Error: eof searched. terminate..."
                    );

                    return false;
                }
            }
        }

        // наконец мы получили, все, что хотели. инициализируем декодеры
        theora_decode_init( &m_theoraState, &m_theoraInfo );

        if( m_theoraInfo.pixelformat != OC_PF_420 )
        {
            const Char * pixelformat[] = { "OC_PF_420", "OC_PF_RSVD", "OC_PF_422", "OC_PF_444" };

            LOGGER_ERROR( "VideoDecoderTheora::_prepareData invalid support pixel format '%s' pls use OC_PF_420"
                , pixelformat[m_theoraInfo.pixelformat]
            );

            return false;
        }

        //if( m_theoraInfo.width != m_theoraInfo.frame_width ||
        //	m_theoraInfo.height != m_theoraInfo.frame_height )
        //{
        //	LOGGER_ERROR("VideoDecoderTheora::_prepareData invalid width or heigth '%d:%d' need '%d:%d' maybe div 16"
        //		, m_theoraInfo.frame_width
        //		, m_theoraInfo.frame_height
        //		, m_theoraInfo.width
        //		, m_theoraInfo.height
        //		);

        //	return false;
        //}

        if( m_options.alpha == true )
        {
            m_dataInfo.frameWidth = m_theoraInfo.frame_width;
            m_dataInfo.frameHeight = m_theoraInfo.frame_height / 2;

            m_dataInfo.width = m_theoraInfo.width;
            m_dataInfo.height = m_theoraInfo.height / 2;

            m_dataInfo.channel = 4;
        }
        else
        {
            m_dataInfo.frameWidth = m_theoraInfo.frame_width;
            m_dataInfo.frameHeight = m_theoraInfo.frame_height;

            m_dataInfo.width = m_theoraInfo.width;
            m_dataInfo.height = m_theoraInfo.height;

            m_dataInfo.channel = 3;
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

        memset( &m_oggStreamState, 0, sizeof( m_oggStreamState ) );
        memset( &m_oggSyncState, 0, sizeof( m_oggSyncState ) );
        memset( &m_theoraState, 0, sizeof( m_theoraState ) );
        memset( &m_theoraComment, 0, sizeof( m_theoraComment ) );
        memset( &m_theoraInfo, 0, sizeof( m_theoraInfo ) );

        ogg_sync_init( &m_oggSyncState );

        // init supporting Theora structures needed in header parsing
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
    size_t TheoraVideoDecoder::_decode( void * _buffer, size_t _bufferSize )
    {
        if( m_readyFrame == false )
        {
            float time;
            if( this->readNextFrame( 0.f, time ) == VDRS_FAILURE )
            {
                return 0;
            }

            m_readyFrame = true;
        }

        yuv_buffer yuvBuffer;
        // декодируем страничку в YUV-виде в спец. структуру yuv_buffer
        int32_t error_code = theora_decode_YUVout( &m_theoraState, &yuvBuffer );

        if( error_code < 0 )
        {
            // ошибка декодирования
            LOGGER_ERROR( "error during theora_decode_YUVout '%d'"
                , error_code
            );

            return 0;
        }

        uint8_t * byte_buffer = static_cast<uint8_t *>(_buffer);

        if( this->decodeBuffer_( yuvBuffer, byte_buffer, _bufferSize ) == false )
        {
            return 0;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TheoraVideoDecoder::decodeBuffer_( const yuv_buffer & _yuvBuffer, uint8_t * _buffer, size_t _size )
    {
        MENGINE_UNUSED( _size );

        if( m_options.alpha == false && m_options.pixelFormat == PF_X8R8G8B8 )
        {
            uint8_t * dstBitmap = _buffer;
            uint8_t * dstBitmapOffset = _buffer + m_pitch;

            uint32_t dstOff = m_pitch * 2 - m_theoraInfo.width * 4;
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t*)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t*)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t*)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            //Loop does four blocks per iteration (2 rows, 2 pixels at a time)
            for( int32_t y = 0; y != y_height; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    //Get uv pointers for row
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    //get corresponding lookup values					
                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    //scale down - brings are values back into the 8 bits of a byte
                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    dstBitmap[THEORA_COLOR_A] = 255;
                    ++ySrc;

                    //And repeat for other pixels (note, y is unique for each
                    //pixel, while uv are not)
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

                    //Advance inner loop offsets
                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                } // end for x

                //Advance destination pointers by offsets
                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            } //end for y
        }
        else if( m_options.alpha == false && m_options.pixelFormat == PF_R8G8B8 )
        {
            uint8_t * dstBitmap = _buffer;
            uint8_t * dstBitmapOffset = _buffer + m_pitch;

            uint32_t dstOff = m_pitch * 2 - m_theoraInfo.width * 3;
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t*)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t*)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t*)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            //Loop does four blocks per iteration (2 rows, 2 pixels at a time)
            for( int32_t y = 0; y != y_height; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    //Get uv pointers for row
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    //get corresponding lookup values

                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    //scale down - brings are values back into the 8 bits of a byte
                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    ++ySrc;

                    //And repeat for other pixels (note, y is unique for each
                    //pixel, while uv are not)
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

                    //Advance inner loop offsets
                    dstBitmap += 3 << 1;
                    dstBitmapOffset += 3 << 1;
                } // end for x

                //Advance destination pointers by offsets
                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            } //end for y
        }
        else if( m_options.alpha == true )
        {
            uint8_t * dstBitmap = _buffer;
            uint8_t * dstBitmapOffset = _buffer + m_pitch;

            uint32_t dstOff = m_pitch * 2 - m_theoraInfo.width * 4;//m_theoraInfo.width * 4;//( m_Width*6 ) - ( yuv->y_width*3 );
            int32_t yOff = (_yuvBuffer.y_stride * 2) - _yuvBuffer.y_width;

            int32_t y_height = _yuvBuffer.y_height >> 1;
            int32_t y_width = _yuvBuffer.y_width >> 1;

            uint8_t * ySrc = (uint8_t*)_yuvBuffer.y;
            uint8_t * uSrc = (uint8_t*)_yuvBuffer.u;
            uint8_t * vSrc = (uint8_t*)_yuvBuffer.v;
            uint8_t * ySrc2 = ySrc + _yuvBuffer.y_stride;

            //Loop does four blocks per iteration (2 rows, 2 pixels at a time)
            //int y_rgb_height_begin = y_height;
            //int y_rgb_height_end = y_height / 2;
            int32_t y_rgb_count = y_height / 2;

            for( int32_t y = 0; y != y_rgb_count; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    //Get uv pointers for row
                    int32_t u = uSrc[x];
                    int32_t v = vSrc[x];

                    //get corresponding lookup values					
                    int32_t rV = s_RVTable[v];
                    int32_t gUV = s_GUTable[u] + s_GVTable[v];
                    int32_t bU = s_BUTable[u];

                    //scale down - brings are values back into the 8 bits of a byte
                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    int32_t g = (rgbY - gUV) >> 13;
                    int32_t b = (rgbY + bU) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_R] );
                    THEORA_CLIP_RGB_COLOR( g, dstBitmap[THEORA_COLOR_G] );
                    THEORA_CLIP_RGB_COLOR( b, dstBitmap[THEORA_COLOR_B] );
                    ++ySrc;

                    //And repeat for other pixels (note, y is unique for each
                    //pixel, while uv are not)
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

                    //Advance inner loop offsets
                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                } // end for x

                //Advance destination pointers by offsets
                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                uSrc += _yuvBuffer.uv_stride;
                vSrc += _yuvBuffer.uv_stride;
            } //end for y

            dstBitmap = _buffer;
            dstBitmapOffset = _buffer + m_pitch;

            for( int32_t y = 0; y != y_rgb_count; ++y )
            {
                for( int32_t x = 0; x != y_width; ++x )
                {
                    //Get uv pointers for row
                    int32_t v = vSrc[x];

                    //get corresponding lookup values

                    int32_t rV = s_RVTable[v];

                    //scale down - brings are values back into the 8 bits of a byte
                    int32_t rgbY = s_YTable[*ySrc];
                    int32_t r = (rgbY + rV) >> 13;
                    THEORA_CLIP_RGB_COLOR( r, dstBitmap[THEORA_COLOR_A] );
                    ++ySrc;

                    //And repeat for other pixels (note, y is unique for each
                    //pixel, while uv are not)
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

                    //Advance inner loop offsets
                    dstBitmap += 4 << 1;
                    dstBitmapOffset += 4 << 1;
                } // end for x

                //Advance destination pointers by offsets
                dstBitmap += dstOff;
                dstBitmapOffset += dstOff;
                ySrc += yOff;
                ySrc2 += yOff;
                vSrc += _yuvBuffer.uv_stride;
            } //end for y
        }
        else
        {
            LOGGER_ERROR( "VideoDecoderTheora::decodeBuffer_ unsupport format" );

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
    EVideoDecoderReadState TheoraVideoDecoder::readNextFrame( float _request, float & _pts )
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

        _pts = m_time;

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
    void TheoraVideoDecoder::setPitch( size_t _pitch )
    {
        m_pitch = (uint32_t)_pitch;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t TheoraVideoDecoder::getPitch() const
    {
        return (size_t)m_pitch;
    }
    //////////////////////////////////////////////////////////////////////////
}
