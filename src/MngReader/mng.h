#	pragma once

#	include <vector>

namespace	mnglib
{
	struct Frame 
	{
		int	index;
		float	delay;
	};

	struct Image
	{
		float	offsetX;
		float	offsetY;
		int	size;
		unsigned char* buffer;
	};

	struct mngDesc  
	{
		int	width;
		int	height;
		std::vector<Frame>	frames;
		std::vector<Image>	images;
	};

	void	readMNG(mngDesc& _desc, void* _buffer, size_t _size);
	void	freeMNG(mngDesc& _desc);
};