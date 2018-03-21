#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{    	
#	ifndef MENGINE_POSIX_FILE_BUFFER_SIZE
#	define MENGINE_POSIX_FILE_BUFFER_SIZE 8192
#	endif

	class PosixFileInputStream
		: public FileInputStreamInterface
	{
	public:
		PosixFileInputStream();
		~PosixFileInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size ) override;
		
	public:
		size_t read( void* _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		bool skip( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;
		bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

	public:
		bool memory( void ** _memory, size_t * _size ) override;

    public:
        void _destroy() override;

	protected:
		bool read_( void * _buf, size_t _size, size_t & _read );

    private:
        bool concatenateFilePath(const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity) const;

	private:
        ServiceProviderInterface * m_serviceProvider;

		FILE * m_hFile;

		Menge::FilePath m_folder;
		Menge::FilePath m_filename;
				
        size_t m_size;
		size_t m_offset;

		size_t m_carriage;
		size_t m_capacity;
		size_t m_reading;

		uint8_t m_readCache[MENGINE_POSIX_FILE_BUFFER_SIZE];
	};
}	// namespace Menge
