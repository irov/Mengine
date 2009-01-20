#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class DataStreamInterface;

	struct DecoderDesc
	{
		DataStreamInterface * stream;
		String type;
	};
}

#	define DECODER_IMPLEMENT( C )\
	void * C::genObject( void * _data )\
	{\
	DecoderDesc * desc = (DecoderDesc *)_data;\
	return new C( desc->stream, desc->type );\
	}\
	const Menge::String & C::getType() const { return m_type; }
