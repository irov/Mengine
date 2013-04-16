#	pragma once

#	include "Interface/FileSystemInterface.h"

#   include <s3eFile.h>

namespace Menge
{
	class MarmaladeOutputStream
		: public FileOutputStreamInterface
	{
	public:
		MarmaladeOutputStream( ServiceProviderInterface * _serviceProvider );
		~MarmaladeOutputStream();

	public:
		bool open( const FilePath& _filename ) override;

	public:
		bool write( const void * _data, size_t _count ) override;
		void flush() override;

    public:
        void destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;
        
		s3eFile * m_hFile;
	};
}	// namespace Menge
