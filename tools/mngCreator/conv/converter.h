#	pragma once

#	include <vector>

namespace	mnglib
{
	struct mngWriter 
	{
		bool	createMNG(const std::string& _filename, int size);
		void	writeFrame(int _index, int _delay);
		bool	writeImageFromFile(const std::string& _filename);
		bool	closeMNG();
	private: 
		FILE*	m_fp;
		int		m_posInFile;
		static	unsigned char*	buffer;
		static	unsigned long	sizebuf;
		static	void	flush_buffer(png_structp);
		static	void	write_to_buffer(png_structp, png_bytep, png_size_t);
		std::vector<int> m_seeks;
		std::vector<int>::iterator m_currentSeek;
	};
};