#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class VideoConverterFFMPEGToGVF
		: public DevelopmentConverter
	{
	public:
		VideoConverterFFMPEGToGVF();
		~VideoConverterFFMPEGToGVF();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );

	protected:
		bool removeTempDir_( const WString & _path );
		bool createTempDir_( const WString & _path );
    };
}	// namespace Menge
