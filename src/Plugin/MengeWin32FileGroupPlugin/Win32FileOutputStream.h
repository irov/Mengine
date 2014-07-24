#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

namespace Menge
{
	class Win32FileOutputStream
		: public FileOutputStreamInterface
	{
	public:
		Win32FileOutputStream();
		~Win32FileOutputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool open( const FilePath & _folder, const FilePath& _fileName ) override;

	public:
		bool write( const void * _data, size_t _count ) override;
		bool flush() override;

	private:
        ServiceProviderInterface * m_serviceProvider;
        
		HANDLE m_hFile;
	};
}	// namespace Menge
