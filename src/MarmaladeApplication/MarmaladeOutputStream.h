#	pragma once

#	include "Interface/FileSystemInterface.h"

#   include <s3eFile.h>

namespace Menge
{
	class MarmaladeOutputStream
		: public FileOutputStreamInterface
	{
	public:
		MarmaladeOutputStream();
		~MarmaladeOutputStream();

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
        
		s3eFile * m_hFile;
	};
}	// namespace Menge
