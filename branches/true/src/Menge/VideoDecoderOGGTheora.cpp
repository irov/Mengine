/*
 *	VideoDecoderOGGTheora.cpp
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "VideoDecoderOGGTheora.h"
#	include "FileInput.h"
#	include "LogEngine.h"

#	include "FactorableImplement.h"
#	include "Utils.h"

#	define OGG_BUFFER_SIZE 8192

//Defines
#define MAX( a, b ) ((a > b) ? a : b)
#define MIN( a, b ) ((a < b) ? a : b)

#define COLOR_R 2
#define COLOR_G 1
#define COLOR_B 0
#define COLOR_A 3

#define CLIP_RGB_COLOR( rgb_color_test, rgb_char_buffer ) \
	rgb_char_buffer = MAX( MIN(rgb_color_test, 255), 0 )

namespace Menge
{

	signed int VideoDecoderOGGTheora::ms_YTable[ 256 ];
	signed int VideoDecoderOGGTheora::ms_BUTable[ 256 ];
	signed int VideoDecoderOGGTheora::ms_GUTable[ 256 ];
	signed int VideoDecoderOGGTheora::ms_GVTable[ 256 ];
	signed int VideoDecoderOGGTheora::ms_RVTable[ 256 ];

	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT( VideoDecoderOGGTheora );
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderOGGTheora::VideoDecoderOGGTheora()
		: m_eof( true )
		, m_currentFrame(0)
		, m_lastReadBytes(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	VideoDecoderOGGTheora::~VideoDecoderOGGTheora()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderOGGTheora::_initialize()
	{
		if( m_stream != NULL )
		{
			m_valid = readHeader_();
			m_eof = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int VideoDecoderOGGTheora::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		if( m_valid == false )
		{
			return 0;
		}

		// ���������� ��������� � YUV-���� � ����. ��������� yuv_buffer
		if( theora_decode_YUVout(&m_theoraState, &m_yuvBuffer) != 0 )
		{
			// ������ �������������
			MENGE_LOG_ERROR( "error during theora_decode_YUVout..." );
		}

		decodeBuffer_( _buffer, _bufferSize );

		return m_lastReadBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderOGGTheora::readHeader_()
	{
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
				MENGE_LOG_ERROR( "Theora Codec Error: bad file" );
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
					MENGE_LOG_ERROR( "TheoraCodec Error: error during ogg_stream_init" );
					clear_();
					return false;
				}

				// ��������� �������� � �������� �����
				if( ogg_stream_pagein( &oggStreamStateTest, &m_oggPage) != 0 )
				{
					MENGE_LOG_ERROR( "TheoraCodec Error: error during ogg_stream_pagein" );
					clear_();
					return false;
				}
				// ���������� ������ �� ����� ��������� ������ � �����
				if( ogg_stream_packetout( &oggStreamStateTest, &m_oggPacket ) == -1 )
				{
					MENGE_LOG_ERROR( "TheoraCodec Error: error during ogg_stream_packetout" );
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
				MENGE_LOG_ERROR( "TheoraCodec Error: error during ogg_stream_packetout" );
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
					MENGE_LOG_ERROR( "TheoraCodec Error: error during theora_decode_header (corrupt stream)" );
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
					MENGE_LOG_ERROR( "TheoraCodec Error: eof searched. terminate..." );
					clear_();
					return false;
				}
			}
		}

		// ������� �� ��������, ���, ��� ������. �������������� ��������
		theora_decode_init( &m_theoraState, &m_theoraInfo );

		
		m_dataInfo.frame_width = m_theoraInfo.width;
		m_dataInfo.frame_height = m_theoraInfo.height;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderOGGTheora::clear_()
	{
		ogg_stream_clear( &m_oggStreamState );
		theora_clear( &m_theoraState );
		theora_comment_clear( &m_theoraComment );
		theora_info_clear( &m_theoraInfo );

		ogg_sync_clear( &m_oggSyncState );
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderOGGTheora::decodeBuffer_( unsigned char* _buffer, int _pitch )
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
		unsigned int dstOff = _pitch * 2 - m_theoraInfo.width * 4;//m_theoraInfo.width * 4;//( m_Width*6 ) - ( yuv->y_width*3 );
		int yOff = (m_yuvBuffer.y_stride * 2) - m_yuvBuffer.y_width;


		//Check if upside down, if so, reverse buffers and offsets
		if ( m_yuvBuffer.y_height < 0 )
		{
			m_yuvBuffer.y_height = -m_yuvBuffer.y_height;
			ySrc		 += (m_yuvBuffer.y_height - 1) * m_yuvBuffer.y_stride;

			uSrc += ((m_yuvBuffer.y_height / 2) - 1) * m_yuvBuffer.uv_stride;
			vSrc += ((m_yuvBuffer.y_height / 2) - 1) * m_yuvBuffer.uv_stride;

			ySrc2 = ySrc - m_yuvBuffer.y_stride;
			yOff  = -m_yuvBuffer.y_width - ( m_yuvBuffer.y_stride * 2 );

			m_yuvBuffer.uv_stride = -m_yuvBuffer.uv_stride;
		}

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
				CLIP_RGB_COLOR( r, dstBitmap[COLOR_R] );
				CLIP_RGB_COLOR( g, dstBitmap[COLOR_G] );
				CLIP_RGB_COLOR( b, dstBitmap[COLOR_B] );
				dstBitmap[COLOR_A] = 255;

				//And repeat for other pixels (note, y is unique for each
				//pixel, while uv are not)
				rgbY = ms_YTable[*ySrc];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmap[4+COLOR_R] );
				CLIP_RGB_COLOR( g, dstBitmap[4+COLOR_G] );
				CLIP_RGB_COLOR( b, dstBitmap[4+COLOR_B] );
				dstBitmap[4+COLOR_A] = 255;
				++ySrc;

				rgbY = ms_YTable[*ySrc2];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmapOffset[COLOR_R] );
				CLIP_RGB_COLOR( g, dstBitmapOffset[COLOR_G] );
				CLIP_RGB_COLOR( b, dstBitmapOffset[COLOR_B] );
				dstBitmapOffset[COLOR_A] = 255;
				++ySrc2;

				rgbY = ms_YTable[*ySrc2];
				r = (rgbY + rV)  >> 13;
				g = (rgbY - gUV) >> 13;
				b = (rgbY + bU)  >> 13;
				CLIP_RGB_COLOR( r, dstBitmapOffset[4+COLOR_R] );
				CLIP_RGB_COLOR( g, dstBitmapOffset[4+COLOR_G] );
				CLIP_RGB_COLOR( b, dstBitmapOffset[4+COLOR_B] );
				dstBitmapOffset[4+COLOR_A] = 255;
				++ySrc2;

				/*dstBitmap[COLOR_G] = 0;
				dstBitmap[COLOR_G+4] = 0;
				dstBitmapOffset[COLOR_G] = 0;
				dstBitmapOffset[COLOR_G+4] = 0;*/

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
	std::streamsize VideoDecoderOGGTheora::buffer_data_()
	{
		char* buffer = ogg_sync_buffer( &m_oggSyncState, OGG_BUFFER_SIZE );
		std::streamsize bytes = m_stream->read( buffer, OGG_BUFFER_SIZE );	
		ogg_sync_wrote( &m_oggSyncState, bytes );
		return bytes;
	}
	//////////////////////////////////////////////////////////////////////////
	int VideoDecoderOGGTheora::readFrame_()
	{
		m_lastReadBytes = 0;
		// theora processing...
		while( ogg_stream_packetout( &m_oggStreamState, &m_oggPacket ) <= 0 )
		{
			// �� ������� ������ � ���������� ������ theora
			// ���� ��������� ������ �� ����������� ������ � ��������� �� � ���������� �����

			// ������ ������ �� �����
			std::streamsize bytes = buffer_data_();
			if( bytes == 0 )
			{
				m_eof = true;
				break;
			}
			m_lastReadBytes += bytes;
			if( m_lastReadBytes == 0 )
			{
				// ���� ��������, ���������� ��������� ����������� ��������
				// � ����� �� ����������
				//TheoraClose();

				//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
				//FINISHED=true;
				return 0;
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
			MENGE_LOG_ERROR( "error during theora_decode_packetin..." );
		}
		return m_lastReadBytes;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoderOGGTheora::createCoefTables_()
	{
		//used to bring the table into the high side (scale up) so we
		//can maintain high precision and not use floats (FIXED POINT)
		int scale = 1L << 13,	temp;

		for ( signed int i = 0; i < 256; i++ )
		{
			temp = i - 128;

			ms_YTable[i]  = (signed int)((1.164 * scale + 0.5) * (i - 16));	//Calc Y component

			ms_RVTable[i] = (signed int)((1.596 * scale + 0.5) * temp);		//Calc R component

			ms_GUTable[i] = (signed int)((0.391 * scale + 0.5) * temp);		//Calc G u & v components
			ms_GVTable[i] = (signed int)((0.813 * scale + 0.5) * temp);

			ms_BUTable[i] = (signed int)((2.018 * scale + 0.5) * temp);		//Calc B component
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int VideoDecoderOGGTheora::sync( float _timing )
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
	bool VideoDecoderOGGTheora::seek( float _timing )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoDecoderOGGTheora::eof()
	{
		if( m_valid == false )
		{
			return true;
		}

		return Utils::eof( m_stream ) || m_eof;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
