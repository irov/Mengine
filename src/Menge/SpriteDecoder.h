#	pragma once
#	include "Decoder.h"
#	include "RenderSystemInterface.h"
#	include <vector>

namespace Menge
{	
	class	SpriteDecoder	
		:	public Decoder
	{
	public:
		struct SpriteData : public	Decoder::DecoderData
		{
			struct Frame 
			{
				int		index;
				int		delay;
			};
			struct Image
			{
				mt::vec2f offset;
				RenderImageInterface* renderImage;
			};
			std::vector<Frame>	frames;
			std::vector<Image>	images;
		};
	};
};