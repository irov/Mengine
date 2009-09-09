#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class FileOutputInterface;

	class Encoder
		: public Factorable
		, virtual public EncoderInterface
	{
	public:
		Encoder();

	public:
		void initialize( FileOutputInterface * _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String & getType() const;
		FileOutputInterface* getStream();

	protected:
		String m_type;
		FileOutputInterface * m_stream;

		bool m_valid;
	};
}
