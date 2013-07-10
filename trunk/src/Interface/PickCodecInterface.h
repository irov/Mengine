
#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	struct PickCodecDataInfo
		: public CodecDataInfo
	{
		PickCodecDataInfo()
			: width(0)
			, height(0)
			, mipmaplevel(0)
            , mipmapsize(0)
		{
		}

        size_t width;
		size_t height;		
		
        size_t mipmaplevel;
        size_t mipmapsize;
	};

	struct PickCodecOptions
		: public CodecOptions
	{
		int pitch;
	};

	class PickDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const PickCodecDataInfo * getCodecDataInfo() const override = 0;
	};

    typedef stdex::intrusive_ptr<PickDecoderInterface> PickDecoderInterfacePtr;

	class PickEncoderInterface
		: public EncoderInterface
	{
	};

    typedef stdex::intrusive_ptr<PickEncoderInterface> PickEncoderInterfacePtr;
}	
