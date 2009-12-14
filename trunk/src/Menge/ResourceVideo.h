#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

namespace Menge
{
	class VideoDecoder;

	class ResourceVideo
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceVideo )
	public:
		ResourceVideo();
		~ResourceVideo();

	public:
		void loader( XmlElement * _xml ) override;

	public:
		const String & getFilename() const;

		bool sync( float _timing );
		void getRGBData( unsigned char* _buffer, int _pitch );
		const mt::vec2f& getFrameSize() const;
		bool eof();
		void seek( float _timing );

	protected:
		bool _compile() override;
		void _release() override;

		String m_filepath;
		VideoDecoder * m_videoDecoder;
		std::streamsize m_bufferSize;
		mt::vec2f m_frameSize;
	};
}
