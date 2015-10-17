#	pragma once

#	include "AEConfig.h"
#	include "AE_GeneralPlug.h"

#	include "Emitter.h"

namespace Menge
{
#	define ASTRALAX_IO_FLATTEN_OPTIONS_SIZE 2048

	class AstralaxOptions
	{
	public:
		AstralaxOptions();
		~AstralaxOptions();

	public:
		bool readFlatten( const unsigned char * _flatten );
		bool writeFlatten( unsigned char * _flatten );

	public:
		//std::wstring path;

		A_Time duration;

		uint32_t width;
		uint32_t height;

		Menge::Emitter * emitter;
	};
}