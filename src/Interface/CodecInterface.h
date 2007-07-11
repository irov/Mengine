#	pragma once

//#	include "MengeExport.h"

#	include "FileEngine.h"
#	include "SystemDLL.h"

#	include <vector>

class DecoderInterface
{
public:
	struct DecoderData
	{
		virtual ~DecoderData(){};
	};
	virtual DecoderData*	decode(void* _buffer, int size) = 0;
private:
};

class	SpriteDecoder	
:	public DecoderInterface
{
public:
	struct SpriteData : public	DecoderInterface::DecoderData
	{
		struct Frame 
		{
			int		index;
			int		delay;
		};
		struct Image
		{
			void* buffer;
			int size;
			float offsetX;
			float offsetY;
		};
		float width;
		float height;
		std::vector<Frame>	frames;
		std::vector<Image>	images;
	};
};

bool	initInterfaceSystem(DecoderInterface** );
void	releaseInterfaceSystem(DecoderInterface* );