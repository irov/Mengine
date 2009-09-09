#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class FileInputInterface;

	class Decoder
		: public Factorable
		, virtual public DecoderInterface
	{
	public:
		Decoder();

	public:
		void initialize( FileInputInterface * _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String& getType() const;
		FileInputInterface* getStream();

	protected:
		String m_type;
		FileInputInterface * m_stream;

		bool m_valid;
	};
}
