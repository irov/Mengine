
#	pragma once

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageDecoder
		: public ImageDecoderInterface
	{
	public:
		ImageDecoder( DataStreamInterface* _stream, const String& _type )
		: m_stream( _stream )
		, m_type( _type )
		{
		}
		
		const String& getType() const
		{
			return m_type;
		}

		DataStreamInterface* getStream()
		{
			return m_stream;
		}
		

	protected:
		std::string m_type;
		DataStreamInterface* m_stream;
	};
}	// namespace Menge
