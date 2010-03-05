#	pragma once

#	include "Factory/Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
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

	public:
		FileOutputInterface * getStream() override;

	protected:
		String m_type;
		FileOutputInterface * m_stream;

		bool m_valid;
	};
}
