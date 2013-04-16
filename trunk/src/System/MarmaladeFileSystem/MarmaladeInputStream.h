#	pragma once

#	include "Interface/FileSystemInterface.h"

#   include <s3eFile.h>

namespace Menge
{
    class ServiceProviderInterface;

    const uint32 FILE_BUFFER_SIZE = 8192; //8kb

	class MarmaladeInputStream
		: public FileInputStreamInterface
	{
	public:
		MarmaladeInputStream( ServiceProviderInterface * _serviceProvider );
		~MarmaladeInputStream();

	public:
		bool open( const FilePath& _filename ) override;
		
	public:
		size_t read( void* _buf, size_t _count ) override;
		void seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( time_t & _time ) const override;

    public:
        void destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

        FilePath m_filePath;

		s3eFile * m_hFile;
		int32 m_size;

        uint32 m_carriage;
        uint32 m_capacity;
        uint32 m_reading;

        char m_buff[FILE_BUFFER_SIZE];
	};
}	// namespace Menge
