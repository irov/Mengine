#	include "Win32MappedFileInputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32MappedFileInputStream::Win32MappedFileInputStream()
		: m_serviceProvider(nullptr)
		, m_hMapping(INVALID_HANDLE_VALUE)
		, m_memory(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32MappedFileInputStream::~Win32MappedFileInputStream()
	{

	}
    //////////////////////////////////////////////////////////////////////////
    void Win32MappedFileInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32MappedFileInputStream::mapFile( HANDLE _hFile, DWORD _dwAllocationGranularity, size_t _offset, size_t _size )
	{	
		(void)_hFile;
		(void)_dwAllocationGranularity;
		(void)_offset;
		(void)_size;

		return false;
	}
}	// namespace Menge
