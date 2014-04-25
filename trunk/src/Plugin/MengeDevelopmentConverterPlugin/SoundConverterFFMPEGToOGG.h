#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class SoundConverterFFMPEGToOGG
		: public DevelopmentConverter
	{
	public:
		SoundConverterFFMPEGToOGG();
		~SoundConverterFFMPEGToOGG();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	// namespace Menge
