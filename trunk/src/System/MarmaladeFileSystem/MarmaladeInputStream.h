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
		MarmaladeInputStream();
		~MarmaladeInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen ) override;
		
	public:
		size_t read( void* _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;		

    public:
        bool time( uint64 & _time ) const override;

    public:
        void _destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		s3eFile * m_hFile;
		Menge::FilePath m_folder;
		Menge::FilePath m_filename;
		
        size_t m_size;

        uint32 m_carriage;
        uint32 m_capacity;
        uint32 m_reading;

        char m_buff[FILE_BUFFER_SIZE];
	};
}	// namespace Menge
