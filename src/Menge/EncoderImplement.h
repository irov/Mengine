#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class OutStreamInterface;

	struct EncoderDesc
	{
		OutStreamInterface * stream;
		String type;
	};
}

#	define ENCODER_IMPLEMENT( C )\
	void * C::genObject( void * _data )\
{\
	EncoderDesc * desc = (EncoderDesc *)_data;\
	return new C( desc->stream, desc->type );\
}\
	const Menge::String & C::getType() const { return m_type; }
