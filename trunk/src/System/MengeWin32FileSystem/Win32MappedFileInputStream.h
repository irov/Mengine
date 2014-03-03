#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#   include "Core/MemoryProxyInput.h"

namespace Menge
{
	class Win32MappedFileInputStream
		: public MemoryProxyInput
	{
	public:
		Win32MappedFileInputStream();
		~Win32MappedFileInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		       
	public:
		bool mapFile( HANDLE _hFile, DWORD _dwAllocationGranularity, size_t _offset, size_t _size );

	private:
        ServiceProviderInterface * m_serviceProvider;

		HANDLE m_hMapping;
		LPVOID m_memory;
	};
}	// namespace Menge
