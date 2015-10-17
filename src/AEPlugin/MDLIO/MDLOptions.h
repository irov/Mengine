#	pragma once

#	include "AEConfig.h"
#	include "AE_GeneralPlug.h"

#	include "Model.h"

namespace Menge
{
#	define MDL_IO_FLATTEN_OPTIONS_SIZE 2048

	class MDLOptions
	{
	public:
		MDLOptions();
		~MDLOptions();

	public:
		bool readFlatten( const unsigned char * _flatten );
		bool writeFlatten( unsigned char * _flatten );

	public:
		//std::wstring path;

		A_Time duration;

		uint32_t width;
		uint32_t height;

		Menge::Model * model;
	};
}