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

		//�������������� �������� ��������� ���������� ogg
		ogg_stream_clear( &m_oggStreamState );
		// start up Ogg stream synchronization layer
		ogg_sync_init( &m_oggSyncState );

		// init supporting Theora structures needed in header parsing
		theora_comment_init( &m_theoraComment );
		theora_info_init( &m_theoraInfo );

		// Ogg file open; parse the headers
		// Only interested in Vorbis/Theora streams 
		bool stateFlag = false;
		int theoraHeaderPackets = 0;  // ����� ������������ ������� ���������� theora
		while( !stateFlag )
		{
			// � �������� �� � ����� ������ ogg
			if( buffer_data_() == 0 )
			{
				// �������� ����, �� ������ ����� ��� ������
				MENGE_LOG( MENGE_TEXT("Theora Codec Error: bad file") );
				clear_();
				return false;
			}
			// ogg_sync_pageout - ��������� ��������
			while( ogg_sync_pageout( &m_oggSyncState, &m_oggPage ) > 0 )
				// 1-������ ������ �� ���������
				// 0-��������� ������ ������ ��� �������� ��������
			{

				// ��� �������� ������������ �������

				// ��� �������� ����������? ���� ���, ������� ������ ���������
				if( ogg_page_bos(&m_oggPage) == false )
				{
					// ���, ��� �� �������� ����������
					// ������, �������� ���������� ���� ���������� ������� ���������
					// � �������� ������ ���� �������
					// ����� ������� ���� ���������� � ������ ������� ������

					// ���������� ��� ��������� � ���������� ����������
					ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
					// ���������� ���������
					// � ���������� ����� theora, ����
					// ��������� ������������� ����������� ������
					// � ��������� ������ �������� ������������

					// ������� �� ������
					stateFlag = true;
					break;
				}

				// ��, ��� �������� ����������

				// �������� ���������� �����
				ogg_stream_state oggStreamStateTest;
				std::fill( (char*)&oggStreamStateTest, (char*)&oggStreamStateTest + sizeof(ogg_stream_state), 0x00 );

				// �������������� �������� ����� �� ��� �� ����� � ����� ��
				// ��������������� ������, ��� � � ������� ���������
				if( ogg_stream_init( &oggStreamStateTest, ogg_page_serialno(&m_oggPage) ) != 0 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_init") );
					clear_();
					return false;
				}

				// ��������� �������� � �������� �����
				if( ogg_stream_pagein( &oggStreamStateTest, &m_oggPage) != 0 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_pagein") );
					clear_();
					return false;
				}
				// ���������� ������ �� ����� ��������� ������ � �����
				if( ogg_stream_packetout( &oggStreamStateTest, &m_oggPacket ) == -1 )
				{
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_packetout") );
					clear_();
					return false;
				}

				// theoraHeaderPackets - ����� �����������
				// ������������ ������� theora (�� �������)
				// �� ������������ theora ����� ������� ������ ���� ���
				if( theoraHeaderPackets == 0 )
				{
					int dhr = theora_decode_header( &m_theoraInfo, &m_theoraComment, &m_oggPacket );
					// ���������� ��������� theora
					if( dhr < 0 )
					{
						// ��� �� ��������� theora

						// ������� ��������� ��������� ������
						ogg_stream_clear( &oggStreamStateTest );
						//� ���������� ���� � ������� ���������� theora
					}
					else
					{
						// ��� ��������� theora!

						// ��� ����� ������� "��������������" ���������� ����� theora:
						std::copy( (char*)&oggStreamStateTest, (char*)&oggStreamStateTest + sizeof(ogg_stream_state), (char*)&m_oggStreamState );
						// ������ �� ����� ������ ����� ������ �������� ������ theora

						theoraHeaderPackets++;

						// ����� ����, ��� �� ����� ������������ �������� ����������� ������ theora,
						// ��� ���������� ��������� ��� ��������� ������������ ��������
						// ������ ������� � ��������� �� (���� �������, �������, �������)
					}
				}
			}
		}

		// ������ ���� �������� ��� ��� ������ ���������� theora (��. � ������������)
		// � ����� ���������� � ���������� ���������������

		while( theoraHeaderPackets < 3 )
		{
			int result = ogg_stream_packetout( &m_oggStreamState, &m_oggPacket );
			// ���� ������� ���������� ����, ������ �� ������� ������ ��� �������������

			if( result < 0 )
			{
				// ������ �������������, ������������ �����
				MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during ogg_stream_packetout") );
				clear_();
				return false;
			}

			if( result > 0 )
			{
				// ������� ������� ������� ����� ���������� theora
				int result2 = theora_decode_header( &m_theoraInfo, &m_theoraComment, &m_oggPacket );

				if( result2 < 0 )
				{
					// ������ �������������, ������������ �����
					MENGE_LOG( MENGE_TEXT("TheoraCodec Error: error during theora_decode_header (corrupt stream)") );
					clear_();
					return false;
				}

				++theoraHeaderPackets;
			}

			// ��� ��������� ����������, ���� ������� �����
			// ��� ����� ��������� ����� ������, ����� ��� �������� ���-���� �������
			// �� ���������. ���� �� ��������, ����� ������ ������ ��� ������ �� �����:
			if( ogg_sync_pageout( &m_oggSyncState, &m_oggPage ) >0 )
				// ogg_sync_pageout - �������, ����� ������ �� ������ ������ ogg
				// � ���������� �� � ogg_page
			{
				//�� ����� ��������� � ������ �...
				ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
				// ...������ ��� ������ � ���������� �����
			}
			else
			{
				// ������ �� � ������ �� �����
				int ret = buffer_data_();
				// ���� ������ ������! ������ �� �� �����
				if( ret == 0 )
				{
					// ����� ���� ��������!
					MENGE_LOG( MENGE_TEXT( "TheoraCodec Error: eof searched. terminate...") );
					clear_();
					return false;
				}
			}
		}

		// ������� �� ��������, ���, ��� ������. �������������� ��������
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
			// �� ������� ������ � ���������� ������ theora
			// ���� ��������� ������ �� ����������� ������ � ��������� �� � ���������� �����

			// ������ ������ �� �����
			int ret = buffer_data_();
			if( ret == 0 )
			{
				// ���� ��������, ���������� ��������� ����������� ��������
				// � ����� �� ����������
				//TheoraClose();

				//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
				//FINISHED=true;
				return ret;
			}

			while( ogg_sync_pageout(&m_oggSyncState, &m_oggPage ) > 0 )
				// ���������� ������ �� ������ � �������� (ogg_page)
				// ���� ��� �� �������� � ������
			{
				// ������ ��� �������� � �����. ���������� ������
				ogg_stream_pagein( &m_oggStreamState, &m_oggPage );
			}

		}

		// ������ ������������. � ������ ���������� �������������� ogg-����������
		// (�� ���� �������������� theora-����������)

		// ��������� ����� � ������� theora
		if( theora_decode_packetin(&m_theoraState,&m_oggPacket) == OC_BADPACKET)
		{
			// ������ �������������
			MENGE_LOG( MENGE_TEXT("error during theora_decode_packetin...") );
		}

		// ��� ������ ��������, ������� ����

		// ���������� ��������� � YUV-���� � ����. ��������� yuv_buffer
		if( theora_decode_YUVout(&m_theoraState, &m_yuvBuffer) != 0 )
		{
			// ������ �������������
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