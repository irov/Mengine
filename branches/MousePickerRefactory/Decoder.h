#	pragma once

#	include "Factorable.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	class FileInput;

	class Decoder
		: public Factorable
		, virtual public DecoderInterface
	{
	public:
		Decoder();

	public:
		void initialize( FileInput* _stream, const String & _type );

	public:
		virtual void _initialize();

	public:
		const String& getType() const;
		FileInput* getStream();

	protected:
		String m_type;
		FileInput* m_stream;

		bool m_valid;
	};
}
