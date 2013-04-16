#	pragma once

#	include "VideoConverter.h"

namespace Menge
{
    class FileServiceInterface;
    class UnicodeServiceInterface;
    class StringizeServiceInterface;
	class LogServiceInterface;

	class VideoConverterFFMPEGToWEBM
		: public VideoConverter
	{
	public:
		VideoConverterFFMPEGToWEBM( ServiceProviderInterface * _serviceProvider );
		~VideoConverterFFMPEGToWEBM();

	public:
		bool initialize() override;

    public:
        const String & getConvertExt() const override;

    public:
		bool convert( bool & _skip ) override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );

    protected:
        String m_convertExt;
    };
}	// namespace Menge
