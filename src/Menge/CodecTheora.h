#	pragma once

#	include "ImageCodec.h"

#	include "ogg/ogg.h"
#	include "theora/theora.h"

namespace Menge
{
	class ImageCodecTheora
		: public ImageCodec
	{
	public:
		ImageCodecTheora();
		~ImageCodecTheora();

		bool code( OutStreamInterface* _outStream, unsigned char* _buffer, CodecData* _data ) const override;
		bool getDataInfo( DataStreamInterface* _inputData, CodecData* _codecData ) const override;
		bool decode( DataStreamInterface* _input, unsigned char* _buffer, unsigned int _options ) const override;

		bool start( DataStreamInterface* _inputData, CodecData* _codecData ) override;
		int sync( float _timing ) override;
		void finish() override;

		std::streamsize read( void* _buf, std::streamsize _count ) override;
		String getLine( bool _trimAfter = true ) override;
		std::streamsize skipLine(  const String& _delim ) override;
		void seek( std::streamoff _pos ) override;
		std::streampos tell() const override;
		bool eof() const override;
		std::streamsize size() const override;
		void* getBuffer() override;
		void setFreeOnClose( bool _free ) override;

	protected:
		DataStreamInterface* m_stream;

		ogg_stream_state m_oggStreamState;
		ogg_sync_state	m_oggSyncState;
		ogg_page m_oggPage;
		ogg_packet m_oggPacket;

		theora_comment m_theoraComment;
		theora_info m_theoraInfo;
		theora_state m_theoraState;

		yuv_buffer m_yuvBuffer;

		unsigned int m_currentFrame;

		void clear_();
		std::streamsize buffer_data_();
	};
}	// namespace Menge