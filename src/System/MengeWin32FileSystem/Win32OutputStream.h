#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

namespace Menge
{
	class Win32OutputStream
		: public FileOutputStreamInterface
	{
	public:
		Win32OutputStream( ServiceProviderInterface * _serviceProvider );
		~Win32OutputStream();

	public:
		bool open( const FilePath& _filename ) override;

	public:
		bool write( const void * _data, size_t _count ) override;
		void flush() override;

    public:
        void destroy() override;

	private:
        ServiceProviderInterface * m_serviceProvider;
        
		HANDLE m_hFile;
	};
}	// namespace Menge
