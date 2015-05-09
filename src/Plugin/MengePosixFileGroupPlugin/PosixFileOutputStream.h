#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class PosixFileOutputStream
		: public FileOutputStreamInterface
	{
	public:
		PosixFileOutputStream();
		~PosixFileOutputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath& _filename ) override;
        
	public:
		bool write( const void * _data, size_t _count ) override;
		bool flush() override;

    public:
        void _destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		FILE * m_hFile;
	};
}	// namespace Menge
