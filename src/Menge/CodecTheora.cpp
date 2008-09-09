#	include "CodecTheora.h"

#	include "LogEngine.h"

#	define OGG_BUFFER_SIZE 8192
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageCodecTheora::ImageCodecTheora()
		: m_stream( 0 )
	{
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
		float frame_time = theora_granule_time( &m_theoraState, m_theoraState.granulepos ) * 1000.0f;
		if( frame_time < _timing )
		{
			return -1;
		}
		else if( frame_time > _timing )
		{
			return 1;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	std::streamsize ImageCodecTheora::read( void* _buf, std::streamsize _count )
	{
		int ret = 0;
		// theora processing...
		while( ogg_stream_packetout( &m_oggStreamState, &m_oggPacket ) <= 0 )
		{
			// не хватает данных в логическом потоке theora
			// надо надергать данных из физического потока и затолкать их в логический поток

			// читаем данные из файла
			int ret = buffer_data_();
			if( ret == 0 )
			{
				// файл кончился, необходимо выполнить закрывающие действия
				// и выйти из приложения
				//TheoraClose();

				//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
				//FINISHED=true;
				return ret;
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

		// все данные получены, готовим кадр

		// декодируем страничку в YUV-виде в спец. структуру yuv_buffer
		if( theora_decode_YUVout(&m_theoraState, &m_yuvBuffer) != 0 )
		{
			// ошибка декодирования
			MENGE_LOG( MENGE_TEXT("error during theora_decode_YUVout...") );
		}

		return ret;
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
		return false;
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
}	// namespace Menge