#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DataStreamInterface;

	class Encoder
		: public Factorable
		, virtual public EncoderInterface
	{
	public:
		Encoder();

	public:
		void initialize( OutStreamInterface * _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String & getType() const;
		OutStreamInterface* getStream() override;

	protected:
		String m_type;
		OutStreamInterface * m_stream;

		bool m_valid;
	};
}