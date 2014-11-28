#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class VideoConverterFFMPEGToOGV
		: public DevelopmentConverter
	{
	public:
		VideoConverterFFMPEGToOGV();
		~VideoConverterFFMPEGToOGV();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	// namespace Menge
