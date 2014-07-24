#	include "Win32FileMapped.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileMapped::Win32FileMapped()
		: m_serviceProvider(nullptr)
        , m_hFile(INVALID_HANDLE_VALUE)
		, m_hMapping(INVALID_HANDLE_VALUE)
		, m_memory(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileMapped::~Win32FileMapped()
	{       
		if( m_memory != NULL )
		{
			if( UnmapViewOfFile( m_memory ) == FALSE )
			{
				DWORD uError = GetLastError();

				LOGGER_ERROR(m_serviceProvider)("Win32MappedFileInputStream invalid UnmapViewOfFile %p error %d"
					, m_memory
					, uError
					);
			}

			m_memory = NULL;
		}

		if( m_hMapping != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hMapping );
			m_hMapping = INVALID_HANDLE_VALUE;
		}

        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32FileMapped::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileMapped::open( const FilePath & _folder, const FilePath & _fileName )
	{
        WChar filePath[MAX_PATH];
        if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( _folder, _fileName, filePath, MAX_PATH ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open invlalid concatenate filePath '%s':'%s'"
                , _folder.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		m_hFile = WINDOWSLAYER_SERVICE(m_serviceProvider)->createFile( 
            filePath, // file to open
			GENERIC_READ, // open for reading
			FILE_SHARE_READ, // share for reading, exclusive for mapping
			OPEN_EXISTING // existing file only
            );

		if ( m_hFile == INVALID_HANDLE_VALUE)
		{
            LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open %ls invalid open"
                , filePath
                );

			return false;
		}

		m_hMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

		if( m_hMapping == NULL )
		{
			DWORD error = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open invalid create file mapping %ls error %d"
				, filePath
				, error
				);

			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		
			return false;
		}

		m_memory = MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );

		if( m_memory == NULL )
		{
			DWORD error = GetLastError();

			LOGGER_ERROR(m_serviceProvider)("Win32MappedInputStream::open invalid map view of file %ls error %d"
				, filePath
				, error
				);

			::CloseHandle( m_hMapping );
			m_hMapping = INVALID_HANDLE_VALUE;

			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
			
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr Win32FileMapped::createFileStream()
	{
		MemoryProxyInput * memory = m_factoryMemoryProxyInput.createObjectT();

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileMapped::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory )
	{
		MemoryProxyInput * memory = stdex::intrusive_get<MemoryProxyInput>(_stream);
		
		void * memory_buffer = memory->setMemory( m_memory, _offset, _size );
		
		if( _memory != nullptr )
		{
			*_memory = memory_buffer;
		}

		return true;
	}
}	// namespace Menge
