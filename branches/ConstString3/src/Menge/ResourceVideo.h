#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

namespace Menge
{
	class FileInputStreamInterface;
	class VideoDecoderInterface;

	class ResourceVideo
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceVideo )
	public:
		ResourceVideo();
		~ResourceVideo();

	public:
		void loader( BinParser * _parser ) override;

	public:
		const WString & getFilePath() const;

	public:
		bool sync( float _timing );
		void getRGBData( unsigned char* _buffer, int _pitch );
		const mt::vec2f& getFrameSize() const;
		bool eof();
		void seek( float _timing );
		bool isAlpha() const;
		void rewind();

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		WString m_filePath;
		bool m_alpha;
		ConstString m_codecType;
		FileInputStreamInterface * m_videoFile;
		VideoDecoderInterface * m_videoDecoder;

		std::streamsize m_bufferSize;
		mt::vec2f m_frameSize;
	};
}
