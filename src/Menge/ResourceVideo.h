#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

namespace Menge
{
	class CodecInterface;

	class ResourceVideo
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceVideo )
	public:
		ResourceVideo( const ResourceFactoryParam & _params );
		~ResourceVideo();

		void sync( float _timing );
		unsigned char* getRGBData();
		const mt::vec2f& getFrameSize();

		void loader( XmlElement * _xml ) override;
	protected:
		bool _compile() override;
		void _release() override;

		String m_filepath;
		CodecInterface* m_stream;
		unsigned char* m_buffer;
		std::streamsize m_bufferSize;
		mt::vec2f m_frameSize;
	};
}	// namespace Menge
