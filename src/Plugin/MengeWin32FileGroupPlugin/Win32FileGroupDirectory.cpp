#	include "Win32FileGroupDirectory.h"

#	include "Interface/MemoryInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/UnicodeInterface.h"

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
	bool Win32FileGroupDirectory::initialize( const ConstString & _name, const ConstString & _category, const FilePath & _folderPath )
	{
		m_name = _name;
		m_category = _category;
        m_folderPath = _folderPath;

        FilePath baseDirectoryPath;

        if( this->createDirectory( baseDirectoryPath ) == false )
        {
            LOGGER_ERROR("Win32FileGroupDirectory::initialize: invalid create directory '%s'"
                , _folderPath.c_str()
                );

            return false;
        }

        m_factoryInputStream = new FactoryPool<Win32FileInputStream, 8>();
        m_factoryOutputStream = new FactoryPool<Win32FileOutputStream, 4>();
        m_factoryWin32MappedFile = new FactoryPool<Win32FileMapped, 4>();

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
	const FilePath & Win32FileGroupDirectory::getFolderPath() const
	{
		return m_folderPath;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::existFile( const FilePath & _fileName ) const
	{
		WChar filePath[MENGINE_MAX_PATH];
		if( WINDOWSLAYER_SERVICE()
			->concatenateFilePath( m_relationPath, m_folderPath, _fileName, filePath, MENGINE_MAX_PATH ) == false )
		{
			LOGGER_ERROR("Win32FileSystem::existFile invlalid concatenate filePath '%s':'%s'"
				, m_folderPath.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		bool result = WINDOWSLAYER_SERVICE()
			->fileExists( filePath );

        return result;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::existDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );
        accountString.append( '/' );
        
        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->existDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupDirectory::createDirectory( const FilePath & _folderName ) const
    {
        const FilePath & relationPath = this->getRelationPath();
        const FilePath & folderPath = this->getFolderPath();

        PathString accountString;
        accountString.append( relationPath );
        accountString.append( folderPath );
        accountString.append( _folderName );

        WString unicode_folderPath;
        if( Helper::utf8ToUnicode( accountString, unicode_folderPath ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->existDirectory( unicode_folderPath ) == true )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->createDirectory( unicode_folderPath ) == false )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr Win32FileGroupDirectory::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		(void)_fileName;
		(void)_streaming;

		Win32FileInputStream * inputStream = m_factoryInputStream->createObject();
        
		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileGroupDirectory::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		(void)_streaming;

        if( _stream == nullptr )
        {
            LOGGER_ERROR("Win32FileGroupDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FileInputStreamInterface * file = stdex::intrusive_get<FileInputStreamInterface *>(_stream);

		if( file->open( m_relationPath, m_folderPath, _filePath, _offset, _size ) == false )
        {
            LOGGER_ERROR("Win32FileGroupDirectory::openInputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr Win32FileGroupDirectory::createOutputFile()
	{
		Win32FileOutputStream * outputStream = m_factoryOutputStream->createObject();

		return outputStream;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool Win32FileGroupDirectory::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
	{
        if( _stream == nullptr )
        {
            LOGGER_ERROR("Win32FileGroupDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FileOutputStreamInterface * file = stdex::intrusive_get<FileOutputStreamInterface *>(_stream);

        if( file->open( m_relationPath, m_folderPath, _filePath ) == false )
        {
            LOGGER_ERROR("Win32FileGroupDirectory::openOutputFile failed open file '%s':'%s'"
                , m_folderPath.c_str()
                , _filePath.c_str()
                );

            return false;
        }
        		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectory::setRelationPath( const FilePath & _relationPath )
    {
        m_relationPath = _relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileGroupDirectory::getRelationPath() const
    {
        return m_relationPath;
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
