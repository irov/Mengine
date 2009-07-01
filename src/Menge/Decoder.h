#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class DataStreamInterface;

	class Decoder
		: public Factorable
		, virtual public DecoderInterface
	{
	public:
		Decoder();

	public:
		void initialize( DataStreamInterface * _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String& getType() const override;
		DataStreamInterface* getStream() override;		

	protected:
		String m_type;
		DataStreamInterface * m_stream;

		bool m_valid;
	};
}