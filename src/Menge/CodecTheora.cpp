#	include "CodecTheora.h"

#	include "LogEngine.h"

#	define OGG_BUFFER_SIZE 8192

//Defines
#define MAX( a, b ) ((a > b) ? a : b)
#define MIN( a, b ) ((a < b) ? a : b)

//int rgb_color_test, unsigned char rgb_char_buffer - can never be negative
//#define CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) \
//	if( rgb_color_test > 255 )							  \
//		rgb_char_buffer = 255;							  \
//	else if( rgb_color_test >= 0 )						  \
//		rgb_char_buffer = rgb_color_test;				  \
//	else												  \
//		rgb_char_buffer = 0

#define CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) \
	rgb_char_buffer = MAX( MIN(rgb_color_test, 255), 0 )

namespace Menge
{
	unsigned int ImageCodecTheora::ms_YTable[ 256 ];
	unsigned int ImageCodecTheora::ms_BUTable[ 256 ];
	unsigned int ImageCodecTheora::ms_GUTable[ 256 ];
	unsigned int ImageCodecTheora::ms_GVTable[ 256 ];
	unsigned int ImageCodecTheora::ms_RVTable[ 256 ];

	//////////////////////////////////////////////////////////////////////////
	ImageCodecTheora::ImageCodecTheora()
		: m_stream( 0 )
		, m_lastReadBytes( 0 )
	{
		createCoefTables_();
	}
	//////////////////////////////////////////////////////////////////////////
	ImageCodecTheora::~ImageCodecTheora()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecTheora::start( DataStreamInterface* _inputData, CodecData* _codecData )
	{
		m_stream = _inputData;

		std::fill( (char*)&m_oggStreamState, (char*)&m_oggStreamState + sizeof(ogg_stream_state), 0x00 );
		std::fill( (char*)&m_oggSyncState, (char*)&m_oggSyncState + sizeof(ogg_sync_state), 0x00 );
		std::fill( (char*)&m_oggPage, (char*)&m_oggPage + sizeof(ogg_page), 0x00 );
		std::fill( (char*)&m_oggPacket, (char*)&m_oggPacket + sizeof(ogg_packet), 0x00 );

		std::fill( (char*)&m_theoraComment, (char*)&m_theoraComment + sizeof(theora_comment), 0x00 );
		std::fill( (char*)&m_theoraInfo, (char*)&m_theoraInfo + sizeof(theora_info), 0x00 );
		std::fill( (char*)&m_theoraState, (char*)&m_theoraState + sizeof(theora_state), 0x00 );
		std::fill( (char*)&m_yuvBuffer, (char*)&m_yuvBuffer + sizeof(yuv_buffer), 0x00 );

		//инициализируем основную структуру контейнера ogg
		ogg_stream_clear( &m_oggStreamState );
		// start up Ogg stream synchronization layer
		ogg_sync_init( &m_oggSyncState );

		// init supporting Theora structures needed in header parsing
		theora_comment_init( &m_theoraComment );
		theora_info_init( &m_theoraInfo );

		// Ogg file open; parse the headers
		// Only interested in Vorbis/Theora streams 
		bool stateFlag = false;
		int theoraHeaderPackets = 0;  // число обработанных пакетов заголовков theora
		while( !stateFlag )
		{
			// и передает их в буфер приема ogg
			if( buffer_data_() == 0 )
			{
				// кончился файл, на данном этапе это ошибка
				MENGE_LOG( MENGE_TEXT("Theora Codec Error: bad file") );
				clear_();
				return false;
			}
			// ogg_sync_pageout - формирует страницу
			while( ogg_sync_pageout( &m_oggSyncState, &m_oggPage ) > 0 )
				// 1-больше данных не требуется
				// 0-требуется больше данных для создания страницы
			{

				// что страница сформирована успешно

				// это страница заголовков? если нет, кончаем искать заголовки
				if( ogg_page_bos(&m_oggPage) == false )
				{
					// нет, это не страница заголовков
					// значит, страницы заголовков всех логических потоков кончились
					// и начались данные этих потоков
					// таким образом надо переходить к чтению страниц данных

					// закидываем эту страничку в логический видеопоток
					ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
					// закидывает страничку
					// в логический поток theora, если
					// совпадает идентификатор логического потока
					// в противном случае страница игнорируется

					// выходим из циклов
					stateFlag = true;
					break;
				}

				// да, это страница заголовков

				// тестовый логический поток
				ogg_stream_state oggStreamStateTest;
				std::fill( (char*)&oggStreamStateTest, (char*)&oggStreamStateTest + sizeof(ogg_stream_state), 0x00 );

				// инициализируем тестовый поток на тот же поток с таким же
				// идентификатором потока, как и у текущей странички
				if( ogg_stream_init( &oggStreamStateTest, ogg_page_serialno(&m_oggPage) ) != 0 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_init") );
					clear_();
					return false;
				}

				// добавляем страницу в тестовый поток
				if( ogg_stream_pagein( &oggStreamStateTest, &m_oggPage) != 0 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_pagein") );
					clear_();
					return false;
				}
				// декодируем данные из этого тестового потока в пакет
				if( ogg_stream_packetout( &oggStreamStateTest, &m_oggPacket ) == -1 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_packetout") );
					clear_();
					return false;
				}

				// theoraHeaderPackets - число прочитанных
				// заголовочных ПАКЕТОВ theora (не страниц)
				// по спецификации theora таких пакетов должно быть три
				if( theoraHeaderPackets == 0 )
				{
					int dhr = theora_decode_header( &m_theoraInfo, &m_theoraComment, &m_oggPacket );
					// декодируем заголовок theora
					if( dhr < 0 )
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
						std::copy( (char*)&oggStreamStateTest, (char*)&oggStreamStateTest + sizeof(ogg_stream_state), (char*)&m_oggStreamState );
						// теперь из этого потока будут всегда сыпаться пакеты theora

						theoraHeaderPackets++;

						// после того, как мы нашли заголовочную страницу логического потока theora,
						// нам необходимо прочитать все остальные заголовочные страницы
						// других потоков и отбросить их (если таковые, конечно, имеются)
					}
				}
			}
		}

		// сейчас надо получить еще два пакета заголовков theora (см. её документацию)
		// и можно переходить к потоковому воспроизведению

		while( theoraHeaderPackets < 3 )
		{
			int result = ogg_stream_packetout( &m_oggStreamState, &m_oggPacket );
			// если функция возвращает нуль, значит не хватает данных для декодирования

			if( result < 0 )
			{
				// ошибка декодирования, поврежденный поток
				MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_packetout") );
				clear_();
				return false;
			}

			if( result > 0 )
			{
				// удалось успешно извлечь пакет информации theora
				int result2 = theora_decode_header( &m_theoraInfo, &m_theoraComment, &m_oggPacket );

				if( result2 < 0 )
				{
					// ошибка декодирования, поврежденный поток
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during theora_decode_header (corrupt stream)") );
					clear_();
					return false;
				}

				++theoraHeaderPackets;
			}

			// эту страничку обработали, надо извлечь новую
			// для этого проверяем буфер чтения, вдруг там осталось что-нить похожее
			// на страничку. Если не осталось, тогда просто читаем эти данные из файла:
			if( ogg_sync_pageout( &m_oggSyncState, &m_oggPage ) >0 )
				// ogg_sync_pageout - функция, берет данные из буфера приема ogg
				// и записывает их в ogg_page
			{
				//мы нашли страничку в буфере и...
				ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
				// ...пихаем эти данные в подходящий поток
			}
			else
			{
				// ничего мы в буфере не нашли
				int ret = buffer_data_();
				// надо больше данных! читаем их из файла
				if( ret == 0 )
				{
					// опять файл кончился!
					MENGE_LOG( MENGE_TEXT( "TheoraCodec Error: eof searched. terminate...") );
					clear_();
					return false;
				}
			}
		}

		// наконец мы получили, все, что хотели. инициализируем декодеры
		theora_decode_init( &m_theoraState, &m_theoraInfo );

		ImageCodec::ImageData* imageData = static_cast<ImageCodec::ImageData*>( _codecData );
		imageData->width = m_theoraInfo.width;
		imageData->height = m_theoraInfo.height;
		imageData->depth = 1;
		imageData->num_mipmaps = 0;
		imageData->size = 0;
		imageData->flags = 0;
		imageData->format = PF_R8G8B8;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecTheora::buffer_data_()
	{
		char* buffer = ogg_sync_buffer( &m_oggSyncState, OGG_BUFFER_SIZE );
		std::streamsize bytes = m_stream->read( buffer, OGG_BUFFER_SIZE );	
		ogg_sync_wrote( &m_oggSyncState, bytes );
		return bytes;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::clear_()
	{
		ogg_stream_clear( &m_oggStreamState );
		theora_clear( &m_theoraState );
		theora_comment_clear( &m_theoraComment );
		theora_info_clear( &m_theoraInfo );

		ogg_sync_clear( &m_oggSyncState );
	}
	//////////////////////////////////////////////////////////////////////////
	int ImageCodecTheora::sync( float _timing )
	{
		int ret = 0;
		float frame_time = theora_granule_time( &m_theoraState, m_theoraState.granulepos ) * 1000.0f;
		if( frame_time < _timing )
		{
			ret = -1;
		}
		else if( frame_time > _timing )
		{
			ret = 1;
		}
		while( frame_time < _timing )
		{
			readFrame_();
			frame_time = theora_granule_time( &m_theoraState, m_theoraState.granulepos ) * 1000.0f;	
		}
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecTheora::read( void* _buf, std::streamsize _count )
	{
		// все данные получены, готовим кадр

		// декодируем страничку в YUV-виде в спец. структуру yuv_buffer
		if( theora_decode_YUVout(&m_theoraState, &m_yuvBuffer) != 0 )
		{
			// ошибка декодирования
			MENGE_LOG( MENGE_TEXT("error during theora_decode_YUVout...") );
		}

		unsigned char* frame = static_cast<unsigned char*>( _buf );
		decodeBuffer_( frame, _count );

		return m_lastReadBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	String ImageCodecTheora::getLine( bool _trimAfter )
	{
		return String(MENGE_TEXT(""));
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecTheora::skipLine( const String& _delim )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::seek( std::streamoff _pos )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::streampos ImageCodecTheora::tell() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecTheora::eof() const
	{
		return m_stream->eof();
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecTheora::size() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void* ImageCodecTheora::getBuffer()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::setFreeOnClose( bool _free )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecTheora::code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecTheora::getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageCodecTheora::decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::finish()
	{
		clear_();
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::decodeBuffer_( unsigned char* _buffer, int _pitch )
	{
		//Convert 4:2:0 YUV YCrCb to an RGB24 Bitmap
		//convenient pointers
		unsigned char *dstBitmap = _buffer;
		unsigned char *dstBitmapOffset = _buffer + _pitch;

		unsigned char *ySrc = (unsigned char*)m_yuvBuffer.y,
			*uSrc = (unsigned char*)m_yuvBuffer.u,
			*vSrc = (unsigned char*)m_yuvBuffer.v,
			*ySrc2 = ySrc + m_yuvBuffer.y_stride;

		//Calculate buffer offsets
		unsigned int dstOff = _pitch;//m_theoraInfo.width * 4;//( m_Width*6 ) - ( yuv->y_width*3 );
		int yOff = (m_yuvBuffer.y_stride * 2) - m_yuvBuffer.y_width;


		//Check if upside down, if so, reverse buffers and offsets
		/*if ( yuv->y_height < 0 )
		{
			yuv->y_height = -yuv->y_height;
			ySrc		 += (yuv->y_height - 1) * yuv->y_stride;

			uSrc += ((yuv->y_height / 2) - 1) * yuv->uv_stride;
			vSrc += ((yuv->y_height / 2) - 1) * yuv->uv_stride;

			ySrc2 = ySrc - yuv->y_stride;
			yOff  = -yuv->y_width - ( yuv->y_stride * 2 );

			yuv->uv_stride = -yuv->uv_stride;
		}*/

		//Cut width and height in half (uv field is only half y field)
		m_yuvBuffer.y_height = m_yuvBuffer.y_height >> 1;
		m_yuvBuffer.y_width = m_yuvBuffer.y_width >> 1;

		//Convientient temp vars
		signed int r, g, b, u, v, bU, gUV, rV, rgbY;
		int x;

		//Loop does four blocks per iteration (2 rows, 2 pixels at a time)
		for (int y = m_yuvBuffer.y_height; y > 0; --y)
		{
			for (x = 0; x < m_yuvBuffer.y_width; ++x) 
			{
				//Get uv pointers for row
				u = uSrc[x]; 
				v = vSrc[x];

				//get corresponding lookup values
				rgbY= ms_YTable[*ySrc];				
				rV  = ms_RVTable[v];
				gUV = ms_GUTable[u] + ms_GVTable[v];
				bU  = ms_BUTable[u];
				++ySrc;

				//scale down - brings are values back into the 8 bits of a byte
				r = (rgbY + rV ) >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU ) >> 13;

				//Clip to RGB values (255 0)
				CLIP_RGB_COLOR( r, dstBitmap[2] );
				CLIP_RGB_COLOR( g, dstBitmap[1] );
				CLIP_RGB_COLOR( b, dstBitmap[0] );
				dstBitmap[3] = 255;

				//And repeat for other pixels (note, y is unique for each
				//pixel, while uv are not)
				rgbY = ms_YTable[*ySrc];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmap[4+2] );
				CLIP_RGB_COLOR( g, dstBitmap[4+1] );
				CLIP_RGB_COLOR( b, dstBitmap[4] );
				dstBitmap[4+3] = 255;
				++ySrc;

				rgbY = ms_YTable[*ySrc2];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmapOffset[2] );
				CLIP_RGB_COLOR( g, dstBitmapOffset[1] );
				CLIP_RGB_COLOR( b, dstBitmapOffset[0] );
				dstBitmapOffset[3] = 255;
				++ySrc2;

				rgbY = ms_YTable[*ySrc2];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmapOffset[4+2] );
				CLIP_RGB_COLOR( g, dstBitmapOffset[4+1] );
				CLIP_RGB_COLOR( b, dstBitmapOffset[4] );
				dstBitmapOffset[4+3] = 255;
				++ySrc2;

				//Advance inner loop offsets
				dstBitmap += 4 << 1;
				dstBitmapOffset += 4 << 1;
			} // end for x

			//Advance destination pointers by offsets
			dstBitmap		+= dstOff;
			dstBitmapOffset += dstOff;
			ySrc			+= yOff;
			ySrc2			+= yOff;
			uSrc			+= m_yuvBuffer.uv_stride;
			vSrc			+= m_yuvBuffer.uv_stride;
		} //end for y
	}
	//////////////////////////////////////////////////////////////////////////
	int ImageCodecTheora::readFrame_()
	{
		m_lastReadBytes = 0;
		// theora processing...
		while( ogg_stream_packetout( &m_oggStreamState, &m_oggPacket ) <= 0 )
		{
			// не хватает данных в логическом потоке theora
			// надо надергать данных из физического потока и затолкать их в логический поток

			// читаем данные из файла
			m_lastReadBytes += buffer_data_();
			if( m_lastReadBytes == 0 )
			{
				// файл кончился, необходимо выполнить закрывающие действия
				// и выйти из приложения
				//TheoraClose();

				//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
				//FINISHED=true;
				return 0;
			}

			while( ogg_sync_pageout(&m_oggSyncState, &m_oggPage ) > 0 )
				// декодируем данные из буфера в страницы (ogg_page)
				// пока они не кончатся в буфере
			{
				// пихаем эти страницы в соотв. логические потоки
				ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
			}

		}

		// удачно декодировали. в пакете содержится декодированная ogg-информация
		// (то бишь закодированная theora-информация)

		// загружаем пакет в декодер theora
		if( theora_decode_packetin(&m_theoraState,&m_oggPacket) == OC_BADPACKET)
		{
			// ошибка декодирования
			MENGE_LOG( MENGE_TEXT("error during theora_decode_packetin...") );
		}
		return m_lastReadBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecTheora::createCoefTables_()
	{
		//used to bring the table into the high side (scale up) so we
		//can maintain high precision and not use floats (FIXED POINT)
		int scale = 1L << 13,
			temp;

		for ( unsigned int i = 0; i < 256; i++ )
		{
			temp = i - 128;

			ms_YTable[i]  = (unsigned int)((1.164 * scale + 0.5) * (i - 16));	//Calc Y component

			ms_RVTable[i] = (unsigned int)((1.596 * scale + 0.5) * temp);		//Calc R component

			ms_GUTable[i] = (unsigned int)((0.391 * scale + 0.5) * temp);		//Calc G u & v components
			ms_GVTable[i] = (unsigned int)((0.813 * scale + 0.5) * temp);

			ms_BUTable[i] = (unsigned int)((2.018 * scale + 0.5) * temp);		//Calc B component
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge