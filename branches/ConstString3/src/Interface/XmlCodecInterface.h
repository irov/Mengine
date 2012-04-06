
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
		WString pathProtocol;
		WString pathXml;
		WString pathBin;
		//int version;
	};

	class XmlDecoderInterface
		: virtual public DecoderInterface
	{
	public:
		virtual const XmlCodecDataInfo * getCodecDataInfo() const override = 0;
	};
}	// namespace Menge
