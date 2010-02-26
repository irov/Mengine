#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class FileOutput;

	class Encoder
		: public Factorable
		, virtual public EncoderInterface
	{
	public:
		Encoder();

	public:
		void initialize( FileOutput* _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String & getType() const;
		FileOutput* getStream();

	protected:
		String m_type;
		FileOutput* m_stream;

		bool m_valid;
	};
}
