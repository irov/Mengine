#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

namespace Menge
{
	class CodecInterface;
	class DataStreamInterface;

	class ResourceVideo
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceVideo )
	public:
		ResourceVideo( const ResourceFactoryParam & _params );
		~ResourceVideo();

		bool sync( float _timing );
		void getRGBData( unsigned char* _buffer );
		const mt::vec2f& getFrameSize();
		bool eof();
		void seek( float _timing );

		void loader( XmlElement * _xml ) override;
	protected:
		bool _compile() override;
		void _release() override;

		String m_filepath;
		DataStreamInterface* m_filestream;
		CodecInterface* m_stream;
		std::streamsize m_bufferSize;
		mt::vec2f m_frameSize;
	};
}	// namespace Menge
