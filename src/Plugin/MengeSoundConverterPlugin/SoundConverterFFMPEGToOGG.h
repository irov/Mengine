#	pragma once

#	include "SoundConverter.h"

namespace Menge
{
    class FileServiceInterface;
    class UnicodeServiceInterface;
    class StringizeServiceInterface;
	class LogServiceInterface;

	class SoundConverterFFMPEGToOGG
		: public SoundConverter
	{
	public:
		SoundConverterFFMPEGToOGG( ServiceProviderInterface * _serviceProvider );
		~SoundConverterFFMPEGToOGG();

	public:
		bool initialize() override;

    public:
        const String & getConvertExt() const override;

    public:
		bool convert( bool & _skip ) override;
        	
	private:
		bool convert_( const FilePath & _input, const FilePath & _output );

    protected:
        String m_convertExt;
    };
}	// namespace Menge
