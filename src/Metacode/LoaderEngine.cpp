#	include "LoaderEngine.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/XmlCodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ConfigInterface.h"

#	include "Core/MemoryHelper.h"
#	include "Core/FilePath.h"

#	include "metabuf/Metadata.hpp"
#   include "Metacode.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoaderService, Menge::LoaderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoaderEngine::LoaderEngine()		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::_initialize()
	{
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		m_protocolPath = CONFIG_VALUE( m_serviceProvider, "Engine", "ProtocolPath", STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "protocol.xml" ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoaderEngine::_finalize()
	{
		m_archivator = nullptr;

		m_protocolPath.clear();
		m_bufferConstString.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void LoaderEngine::setProtocolPath( const FilePath & _protocolPath )
	{
		m_protocolPath = _protocolPath;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const
	{
        LOGGER_INFO(m_serviceProvider)( "LoaderEngine::load pak '%s:%s'"
            , _pak.c_str()
            , _path.c_str()
            );

		if( _path.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("LoaderEngine::import invalid open bin '%s' path is empty"
				, _pak.c_str()
				);

			return false;
		}

		InputStreamInterfacePtr file_bin;
		if( this->openBin_( _pak, _path, file_bin, _exist ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::import invalid open bin '%s':'%s'"
                , _pak.c_str()
                , _path.c_str()
                );

			return false;
		}
        

		if( file_bin == nullptr )
		{
			return true;
		}

		bool reimport = false;
		bool done = this->importBin_( file_bin, _metadata, &reimport );

#	ifndef MENGINE_MASTER_RELEASE
		if( reimport == true )
		{
            file_bin = nullptr;
			
			PathString cache_path_xml;			
			cache_path_xml += _path;			
			cache_path_xml.replace_last( "xml" );
            			
            ConstString c_cache_path_xml = Helper::stringizeStringLocal( m_serviceProvider, cache_path_xml.c_str(), c_cache_path_xml.size() );

			if( this->makeBin_( _pak, FilePath(c_cache_path_xml), _path ) == false )
			{
                LOGGER_ERROR(m_serviceProvider)("LoaderEngine::import invlid rebild bin %s from xml %s"
                    , _path.c_str()
                    , c_cache_path_xml.c_str()
                    );

				return false;
			}

			file_bin = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( _pak, _path, false );

			done = this->importBin_( file_bin, _metadata, nullptr );
		}
#	endif
        
		return done;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::validation( const ConstString & _pak, const FilePath & _path ) const
	{
		InputStreamInterfacePtr stream =
			FILE_SERVICE( m_serviceProvider )->openInputFile( _pak, _path, false );

		if( stream == nullptr )
		{
			return false;
		}

		size_t size = stream->size();

		if( size == 0 )
		{
			return false;
		}

		uint8_t header_buff[Metabuf::header_size];

		stream->read( header_buff, Metabuf::header_size );

		size_t header_read = 0;
		uint32_t readVersion;
		uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;

		if( Metacode::readHeader( header_buff, Metabuf::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metadata * _metadata, bool * _reimport ) const
	{
		size_t size = _stream->size();

		if( size == 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::importBin_ invalid size (empty)"
                );

			return false;
		}

		uint8_t header_buff[Metabuf::header_size];

        _stream->read( header_buff, Metabuf::header_size );

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;

        if( Metacode::readHeader( header_buff, Metabuf::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol ) == false )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invalid version read %d need %d or protocol %d need %d (Update you protocol file)"
                    , readVersion
                    , needVersion
                    , readProtocol
                    , needProtocol
                    );
            }
            else
            {
                *_reimport = true;
            }   

            return false;
        }

        uint32_t bin_size;
        _stream->read( &bin_size, sizeof(bin_size) );

        uint32_t compress_size;
        _stream->read( &compress_size, sizeof(compress_size) );

		MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, bin_size, __FILE__, __LINE__ );

		if( binary_buffer == nullptr )
		{
			return false;
		}

		uint8_t * binary_memory = binary_buffer->getMemory();
		
        size_t uncompress_size = 0;
        if( ARCHIVE_SERVICE(m_serviceProvider)
			->decompressStream( m_archivator, _stream, compress_size, binary_memory, bin_size, uncompress_size ) == false )
        {
			if( _reimport == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid uncompress"
					);
			}
			else
			{
				*_reimport = true;
			}

            return false;
        }
        
        size_t read_size = 0;

        uint32_t stringCount;
        if( Metacode::readStrings( binary_memory, bin_size, read_size, stringCount ) == false )
        {
            return false;
        }

        m_bufferConstString.resize( stringCount );

        for( TVectorConstString::iterator
            it = m_bufferConstString.begin(),
            it_end = m_bufferConstString.end();
        it != it_end;
        ++it )
        {
            uint32_t stringSize;
            int64_t stringHash;
            const char * str = Metacode::readString( binary_memory, bin_size, read_size, stringSize, stringHash );

            if( str == nullptr )
            {
				LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid read string (error)"
					);

                return false;
            }

            *it = Helper::stringizeStringSizeHash(m_serviceProvider, str, stringSize, stringHash);
        }

        if( _metadata->parseRoot( binary_memory, bin_size, read_size, (void *)this ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid parse (error)"
                );

            return false;
        }

        m_bufferConstString.clear();

        if( read_size != bin_size )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid parse (read != archive)"
                );

            return false;
        }

		return true;
	}
#	ifndef MENGINE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
	{
		PathString cache_path_xml;		
		cache_path_xml += _path;
		cache_path_xml.replace_last( "xml" );
        		
        FilePath c_cache_path_xml = Helper::stringizeFilePath( m_serviceProvider, cache_path_xml );
        
		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, c_cache_path_xml, nullptr ) == false )
		{
			if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, nullptr ) == false )
			{
				_exist = false;

				return false;
			}

			InputStreamInterfacePtr file_bin = 
                FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path, false );

			if( file_bin == nullptr )
			{
				return false;
			}

			_file = file_bin;

			return true;
		}

		_exist = true;

		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, nullptr ) == false )
		{
			if( this->makeBin_( _pak, c_cache_path_xml, _path ) == false )
			{
				_file = nullptr;

				return false;
			}
		}

		InputStreamInterfacePtr file_bin = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pak, _path, false );

		if( file_bin == nullptr )
		{
			return false;
		}

		InputStreamInterfacePtr file_xml = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pak, c_cache_path_xml, false );

		if( file_xml == nullptr )
		{
			return false;
		}

		uint64_t time_xml;
		file_xml->time( time_xml );

		uint64_t time_bin;
		file_bin->time( time_bin );

        file_xml = nullptr;

		if( time_xml > time_bin )
		{
			//Rebild bin file from xml
            file_bin = nullptr;

			if( this->makeBin_( _pak, c_cache_path_xml, _path ) == false )
			{
				_file = nullptr;

				return false;
			}

			file_bin = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( _pak, _path, false );
		}

		_file = file_bin;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::makeBin_( const ConstString & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const
	{
		XmlDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "xml2bin") );

		if( decoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("LoaderEngine::makeBin_ invalid create decoder xml2bin for %s:%s"
				, _pak.c_str()
				, _pathXml.c_str()
				);

			return false;
		}

		if( decoder->prepareData( nullptr ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("LoaderEngine::makeBin_ invalid initialize decoder xml2bin for %s:%s"
				, _pak.c_str()
				, _pathXml.c_str()
				);

			return false;
		}

		XmlCodecOptions options;
        options.pathProtocol = m_protocolPath;
		
        FileGroupInterfacePtr fileGroup = FILE_SERVICE(m_serviceProvider)
            ->getFileGroup( _pak );

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::makeBin_ invalid get file group %s (%s)"
                , _pak.c_str()
                , _pathXml.c_str()
                );

            return false;
        }

		const FilePath & path = fileGroup->getPath();

		options.pathXml = Helper::concatenationFilePath( m_serviceProvider, path, _pathXml );
		options.pathBin = Helper::concatenationFilePath( m_serviceProvider, path, _pathBin );

		if( decoder->setOptions( &options ) == false )
        {
            return false;
        }

		//if( decoder->initialize( m_serviceProvider, 0 ) == false )
		//{
			//decoder->destroy();

			//return false;
		//}

		if( decoder->decode( 0, 0 ) == 0 )
		{
			return false;
		}

		return true;
	}
#	else
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
	{
		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, nullptr ) == false )
		{
			_exist = false;

			return false;
		}

		InputStreamInterfacePtr file_bin = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path, false );

		if( file_bin == nullptr )
		{
			return false;
		}

		_file = file_bin;

		return true;
	}
#	endif
    //////////////////////////////////////////////////////////////////////////
    const ConstString & LoaderEngine::getCacheConstString( uint32_t _index ) const
    {
        return m_bufferConstString[_index];
    }
}
