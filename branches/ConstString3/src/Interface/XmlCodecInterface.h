
#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{
	struct XmlCodecDataInfo
		: public CodecDataInfo
	{
	};

	struct XmlCodecOptions
		: public CodecOptions
	{
		String protocol;
		String pathXml;
		String pathBin;
	};

	class XmlDecoderInterface
		: virtual public DecoderInterface
	{
	public:
		virtual const XmlCodecDataInfo * getCodecDataInfo() const override = 0;
	};
}	// namespace Menge
