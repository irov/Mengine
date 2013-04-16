#	include "LoaderEngine.h"

#	include "metabuf/Metabuf.hpp"
#   include "Metacode.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/XmlCodecInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include "Consts.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoaderService, Menge::LoaderServiceInterface, Menge::LoaderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoaderEngine::LoaderEngine()
		: m_serviceProvider(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void LoaderEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * LoaderEngine::getServiceProvider() const
    {
        return m_serviceProvider;             
    }
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist )
	{
        LOGGER_INFO(m_serviceProvider)( "LoaderEngine::load pak '%s:%s'"
            , _pak.c_str()
            , _path.c_str()
            );

		InputStreamInterface * file_bin = 0;
		
		if( this->openBin_( _pak, _path, &file_bin, _exist ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::import invalid open bin %s"
                , _path.c_str()
                );

			return false;
		}

		if( file_bin == 0 )
		{
			return true;
		}

		bool reimport = false;
		bool done = this->importBin_( file_bin, _metadata, &reimport );

#	ifndef MENGE_MASTER_RELEASE
		if( reimport == true )
		{
			file_bin->destroy();

            static String cache_path_xml;
            
            cache_path_xml.assign( _path.c_str(), _path.size() );

            FilePath::size_type size = cache_path_xml.size();
            cache_path_xml[size-3] = L'x';
            cache_path_xml[size-2] = L'm';
            cache_path_xml[size-1] = L'l';

            FilePath path_xml = Helper::stringizeString( m_serviceProvider, cache_path_xml );

			if( this->makeBin_( _pak, path_xml, _path ) == false )
			{
                LOGGER_ERROR(m_serviceProvider)("LoaderEngine::import invlid rebild bin %s from xml %s"
                    , _path.c_str()
                    , path_xml.c_str()
                    );

				return false;
			}

			file_bin = FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path );

			done = this->importBin_( file_bin, _metadata, NULL );
		}
#	endif

		file_bin->destroy();

		return done;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::importBin_( InputStreamInterface * _bin, Metabuf::Metadata * _metadata, bool * _reimport )
	{
		int size = _bin->size();

		if( size == 0 )
		{
			return true;
		}

		m_bufferArchive.resize( size );

		int reading = _bin->read( &m_bufferArchive.front(), size );

		if( reading != size )
		{
			return false;
		}

        size_t read_size = 0;

        size_t readVersion = 0;
        size_t needVersion = 0;

        TBlobject::value_type * archiveBuff = &m_bufferArchive[0];
        size_t archiveSize = m_bufferArchive.size();

        if( Metacode::readHeader( archiveBuff, archiveSize, read_size, readVersion, needVersion, (void *)m_serviceProvider ) == false )
        {
            if( _reimport == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid version read %d need %d"
                    , readVersion
                    , needVersion
                    );
            }
            else
            {
                *_reimport = true;
            }

            return false;
        }

        size_t stringCount;
        if( Metacode::readStrings( archiveBuff, archiveSize, read_size, stringCount, (void *)m_serviceProvider ) == false )
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
            size_t stringSize;
            const char * str = Metacode::readString( archiveBuff, archiveSize, read_size, stringSize, (void *)m_serviceProvider );

            if( str == NULL )
            {
                return false;
            }

            *it = Helper::stringizeStringSize(m_serviceProvider, str, stringSize);
        }

        if( _metadata->parse( archiveBuff, archiveSize, read_size, (void *)this ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid parse (error)"
                );

            return false;
        }

        m_bufferConstString.clear();

        if( read_size != archiveSize )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::loadBinary invlid parse (read != archive)"
                );

            return false;
        }

		return true;
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterface ** _file, bool & _exist )
	{
		static String cache_path_xml;
        
        cache_path_xml.assign( _path.c_str(), _path.size() );

        FilePath::size_type size = cache_path_xml.size();
        cache_path_xml[size-3] = L'x';
        cache_path_xml[size-2] = L'm';
        cache_path_xml[size-1] = L'l';
        
        FilePath path_xml = Helper::stringizeString( m_serviceProvider, cache_path_xml );

		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, path_xml, NULL ) == false )
		{
			if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, NULL ) == false )
			{
				_exist = false;

				return false;
			}

			InputStreamInterface * file_bin = 
                FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path );

			if( file_bin == 0 )
			{
				return false;
			}

			*_file = file_bin;

			return true;
		}

		_exist = true;

		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, NULL ) == false )
		{
			if( this->makeBin_( _pak, path_xml, _path ) == false )
			{
				*_file = 0;

				return false;
			}
		}

		InputStreamInterface * file_bin = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path );

		if( file_bin == 0 )
		{
			return false;
		}

		InputStreamInterface * file_xml = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, path_xml );

		if( file_xml == 0 )
		{
			file_bin->destroy();

			return false;
		}

		time_t time_xml;
		file_xml->time( time_xml );

		time_t time_bin;
		file_bin->time( time_bin );

		file_xml->destroy();

		if( time_xml > time_bin )
		{
			//Rebild bin file from xml
			file_bin->destroy();

			if( this->makeBin_( _pak, path_xml, _path ) == false )
			{
				*_file = 0;

				return false;
			}

			file_bin = FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path );
		}

		*_file = file_bin;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::makeBin_( const ConstString & _pak, const FilePath & _pathXml, const FilePath & _pathBin )
	{
		XmlDecoderInterface * decoder = CODEC_SERVICE(m_serviceProvider)->createDecoderT<XmlDecoderInterface>( Helper::stringizeString(m_serviceProvider, "xml2bin"), 0 );

		if( decoder == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("LoaderEngine::makeBin_ invalid create decoder xml2bin for %s:%s"
				, _pak.c_str()
				, _pathXml.c_str()
				);

			return false;
		}

		XmlCodecOptions options;
        options.pathProtocol = Helper::stringizeString( m_serviceProvider, "protocol.xml" );


        FileGroupInterface * fileGroup = FILE_SERVICE(m_serviceProvider)
            ->getFileSystem( _pak );

        if( fileGroup == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("LoaderEngine::makeBin_ invalid get file group %s (%s)"
                , _pak.c_str()
                , _pathXml.c_str()
                );

            return false;
        }

		const FilePath & path = fileGroup->getPath();
			
        options.pathXml = concatenationFilePath( m_serviceProvider, path, _pathXml );
		options.pathBin = concatenationFilePath( m_serviceProvider, path, _pathBin );

		if( decoder->setOptions( &options ) == false )
        {
            decoder->destroy();

            return false;
        }

		//if( decoder->initialize( m_serviceProvider, 0 ) == false )
		//{
			//decoder->destroy();

			//return false;
		//}

		if( decoder->decode( 0, 0 ) == 0 )
		{
			decoder->destroy();

			return false;
		}

		decoder->destroy();

		return true;
	}
#	else
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterface ** _file, bool & _exist )
	{
		if( FILE_SERVICE(m_serviceProvider)->existFile( _pak, _path, NULL ) == false )
		{
			_exist = false;

			return false;
		}

		InputStreamInterface * file_bin = 
            FILE_SERVICE(m_serviceProvider)->openInputFile( _pak, _path );

		if( file_bin == 0 )
		{
			return false;
		}

		*_file = file_bin;

		return true;
	}
#	endif
    //////////////////////////////////////////////////////////////////////////
    const ConstString & LoaderEngine::getCacheConstString( size_t _index ) const
    {
        return m_bufferConstString[_index];
    }
}