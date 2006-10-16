#	pragma once

#	include "png.h"
#	include <string>
#	include <vector>

namespace	mnglib
{
	struct mngWriter 
	{
		bool	createMNG(const std::string& _filename);
		void	writeFrame(int _index, int _delay);
		bool	writeImageFromFile(const std::string& _filename);
		bool	closeMNG();
	private: 
		FILE*	m_fp;
		int		m_numSprites;
		int		m_posInFile;
		static	unsigned char*	buffer;
		static	unsigned long	sizebuf;
		static	void	flush_buffer(png_structp);
		static	void	write_to_buffer(png_structp, png_bytep, png_size_t);
	};
};