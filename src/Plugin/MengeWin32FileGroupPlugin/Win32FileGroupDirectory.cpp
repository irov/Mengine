#	include "Win32FileGroupDirectory.h"

#	include "Interface/MemoryInterface.h"

#   include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

#	include "WIN32/WindowsIncluder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupDirectory::Win32FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileGroupDirectory::~Win32FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::initialize( const ConstString & _name, const ConstString & _category, const FilePath & _path )
	{
		m_name = _name;
		m_category = _category;
        m_path = _path;

        m_factoryInputStream = new FactoryPool<Win32FileInputStream, 8>( m_serviceProvider );
        m_factoryOutputStream = new FactoryPool<Win32FileOutputStream, 4>( m_serviceProvider );
        m_factoryWin32MappedFile = new FactoryPool<Win32FileMapped, 4>( m_serviceProvider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::finalize()
    {
        m_factoryInputStream = nullptr;
        m_factoryOutputStream = nullptr;
        m_factoryWin32MappedFile = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::isPacked() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & Win32FileGroupDirectory::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE(m_serviceProvider)
			->concatenateFilePath( m_path, _fileName, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Win32FileSystem::existFile invlalid concatenate filePath '%s':'%s'"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		bool result = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->fileExists( filePath );

        return result;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		(void)_fileName;
		(void)_streaming;

		Win32FileInputStream * inputStream = m_factoryInputStream->createObject();

		inputStream->setServiceProvider( m_serviceProvider );

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		(void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>(_stream);

		if( file->open( m_path, _fileName, _offset, _size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile()
	{
		Win32FileOutputStream * outputStream = m_factoryOutputStream->createObject();

		outputStream->setServiceProvider( m_serviceProvider );

		return outputStream;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool Win32FileGroupDirectory::openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream )
	{
        if( _stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>(_stream);

        if( file->open( m_path, _fileName ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_path.c_str()
                , _fileName.c_str()
                );

            return false;
        }
        		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
