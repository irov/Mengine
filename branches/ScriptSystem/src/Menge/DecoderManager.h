#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Factory/FactoryManager.h"

namespace Menge 
{
	class Decoder;
	class FileInput;

	class DecoderManager
		: public FactoryManager
		, public Holder<DecoderManager>
		, public DecoderManagerInterface
	{
	};
} // namespace Menge
