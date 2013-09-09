#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
    class FileServiceInterface;
    class UnicodeServiceInterface;
    class StringizeServiceInterface;
	class LogServiceInterface;

	class VideoConverterFFMPEGToWEBM
		: public DevelopmentConverter
	{
	public:
		VideoConverterFFMPEGToWEBM( ServiceProviderInterface * _serviceProvider );
		~VideoConverterFFMPEGToWEBM();

	public:
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	// namespace Menge
