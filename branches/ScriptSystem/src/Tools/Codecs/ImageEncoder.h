
#	pragma once

#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	class ImageEncoder
		: public ImageEncoderInterface
	{
	public:
		ImageEncoder( OutStreamInterface* _stream, const String& _type )
		: m_stream( _stream )
		, m_type( _type )
		{
		}
		
		const String& getType() const
		{
			return m_type;
		}

		OutStreamInterface* getStream()
		{
			return m_stream;
		}
		

	protected:
		std::string m_type;
		OutStreamInterface* m_stream;
	};
}	// namespace Menge
