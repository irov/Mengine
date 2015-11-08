#	pragma once

#	include "Interface/FileSystemInterface.h"

#   include <s3eFile.h>

namespace Menge
{
	class MarmaladeFileOutputStream
		: public FileOutputStreamInterface
	{
	public:
		MarmaladeFileOutputStream();
		~MarmaladeFileOutputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath& _filename ) override;
        
	public:
		bool write( const void * _data, size_t _count ) override;
		size_t size() const override;

	public:
		bool flush() override;

    public:
        void _destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;
        
		s3eFile * m_hFile;

		size_t m_size;
	};
}	// namespace Menge
