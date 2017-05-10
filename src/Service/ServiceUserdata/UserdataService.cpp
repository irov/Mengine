#	include "UserdataService.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Core/Stream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UserdataService, Menge::UserdataService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	UserdataService::UserdataService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	UserdataService::~UserdataService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool UserdataService::_initialize()
	{
		m_archivator = ARCHIVE_SERVICE( m_serviceProvider )
			->getArchivator( STRINGIZE_STRING_LOCAL( m_serviceProvider, "lz4" ) );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void UserdataService::_finalize()
	{			
	}
	//////////////////////////////////////////////////////////////////////////
	bool UserdataService::addUserdata( const ConstString & _name, const ConstString & _category, const FilePath & _path )
	{
		TMapDatas::const_iterator it_found = m_datas.find( _name );

		if( it_found != m_datas.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::addData data %s already exist"
				, _name.c_str()
				);

			return false;
		}

		UserdataDesc desc;
		desc.category = _category;
		desc.path = _path;

		m_datas.insert( std::make_pair( _name, desc ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool UserdataService::removeUserdata( const ConstString & _name )
	{
		TMapDatas::iterator it_found = m_datas.find( _name );

		if( it_found == m_datas.end() )
		{
			return false;
		}

		m_datas.erase( it_found );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool UserdataService::hasUserdata( const ConstString & _name ) const
	{
		TMapDatas::const_iterator it_found = m_datas.find( _name );

		if( it_found == m_datas.end() )
		{
			return false;
		}

		const UserdataDesc & desc = it_found->second;

		if( FILE_SERVICE( m_serviceProvider )
			->existFile( desc.category, desc.path, nullptr ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr UserdataService::loadUserdata( const ConstString & _name ) const
	{
		TMapDatas::const_iterator it_found = m_datas.find( _name );

		if( it_found == m_datas.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::loadData: data %s not found"
				, _name.c_str()
				);

			return nullptr;
		}

		const UserdataDesc & desc = it_found->second;

		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( desc.category, desc.path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::loadData: data %s invalid open file %s"
				, _name.c_str()
				, desc.path.c_str()
				);

			return nullptr;
		}

        MemoryInterfacePtr binaryBuffer;
		if( Helper::loadStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_USER_DATA ), GET_MAGIC_VERSION( MAGIC_USER_DATA ), binaryBuffer, __FILE__, __LINE__ ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::loadData: data %s invalid load stream archive %s"
				, _name.c_str()
				, desc.path.c_str()
				);

			return nullptr;
		}

		return binaryBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool UserdataService::writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const
	{
		TMapDatas::const_iterator it_found = m_datas.find( _name );

		if( it_found == m_datas.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::writeData: data %s not found"
				, _name.c_str()
				);

			return false;
		}

		const UserdataDesc & desc = it_found->second;

		if( _data == nullptr || _size == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::writeData: data %s write empty file %s"
				, _name.c_str()
				, desc.path.c_str()
				);

			return false;
		}

		OutputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openOutputFile( desc.category, desc.path );

		if( stream == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::writeData: data %s invalid open file %s"
				, _name.c_str()
				, desc.path.c_str()
				);

			return false;
		}

		const void * data_memory = _data;
		size_t data_size = _size;

		if( Helper::writeStreamArchiveData( m_serviceProvider, stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_USER_DATA ), GET_MAGIC_VERSION( MAGIC_USER_DATA ), true, data_memory, data_size, EAC_NORMAL ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("UserdataService::writeData: data %s invalid write file %s"
				, _name.c_str()
				, desc.path.c_str()
				);

			return false;
		}

		return true;
	}
}