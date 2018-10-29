#pragma once

#include "Interface/CodecFactoryInterface.h"
#include "Interface/DecoderInterface.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class DecoderFactoryInterface
		: public CodecFactoryInterface
	{
	public:
		virtual DecoderInterfacePtr createDecoder() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<DecoderFactoryInterface> DecoderFactoryInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
}

