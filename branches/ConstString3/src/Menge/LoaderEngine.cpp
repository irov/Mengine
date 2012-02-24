#	include "LoaderEngine.h"
#	include "Loadable.h"

#	include "BinParser.h"
#	include "FileEngine.h"

#	include "CodecEngine.h"
#	include "Interface/XmlCodecInterface.h"

#	include "Consts.h"

#   include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class LoadableListener
			: public BinParserListener
		{
		public:
			LoadableListener( Loadable * _loadable )
				: m_loadable(_loadable)
			{
			}

		protected:
			void onElement( BinParser * _parser ) override
			{
				m_loadable->loader( _parser );
			}

			void onEndElement() override
			{
				m_loadable->loaded();
			}

		protected:
			Loadable * m_loadable;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	LoaderEngine::LoaderEngine()
		: m_bufferLevel(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class FFindBlobject
		{
		public:
			FFindBlobject( const ConstString & _pak, const String & _path )
				: m_pak(_pak)
				, m_path(_path)
			{

			}

		public:
			bool operator() ( const BlobjectCache & _cache )
			{
				if( _cache.pak != m_pak )
				{
					return false;
				}

				if( _cache.path != m_path )
				{
					return false;
				}

				return true;
			}

		protected:
			const ConstString & m_pak;
			const String & m_path;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::cache( const ConstString & _pak, const String & _path, Loadable * _loadable, bool & _exist )
	{
		TBlobjectCache::iterator it_found = std::find_if( m_cache.begin(), m_cache.end(), FFindBlobject(_pak, _path) ); 

		if( it_found == m_cache.end() )
		{
			BlobjectCache cache;
			cache.pak = _pak;
			cache.path = _path;			

			if( this->import( cache.pak, cache.path, cache.blob, _exist ) == false )
			{
				if( _exist == true )
				{
					return false;
				}
			}

			it_found = m_cache.insert( m_cache.begin(), cache );

			if( m_cache.size() > 10 )
			{
				m_cache.pop_back();
			}
		}

		const BlobjectCache & cache = *it_found;

		if( cache.blob.empty() == true )
		{
			_exist = false;

			_loadable->loaded();

			return true;
		}

		_exist = true;

		if( LoaderEngine::get()
			->loadBinary( cache.blob, _loadable ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::load( const ConstString & _pak, const String & _path, Loadable * _loadable, bool & _exist )
	{
        MENGE_LOG_INFO( "LoaderEngine::load pak '%s' path '%s'"
                       , _pak.c_str()
                       , _path.c_str()
                       );
        
		Archive & buffer = m_bufferArchive[m_bufferLevel];

		++m_bufferLevel;

		if( this->import( _pak, _path, buffer, _exist ) == false )
		{
			--m_bufferLevel;

			return false;
		}

        
		if( this->loadBinary( buffer, _loadable ) == false )
		{
			--m_bufferLevel;
			return false;
		}

		--m_bufferLevel;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::loadBinary( const Archive & _blob, Loadable * _loadable )
	{
		BinParser parser( _blob.begin(), _blob.end() );

		LoadableListener listener(_loadable);
		if( parser.run( &listener ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::import( const ConstString & _pak, const String & _path, Archive & _archive, bool & _exist )
	{
		FileInputStreamInterface * file_bin;
		
		if( this->openBin_( _pak, _path, &file_bin, _exist ) == false )
		{
			return false;
		}

		if( file_bin == 0 )
		{
			return true;
		}


		bool reimport = false;
		bool done = this->importBin_( file_bin, _archive, reimport );

#	ifndef MENGE_MASTER_RELEASE
		if( reimport == true )
		{
			file_bin->close();

			String path_xml = _path + ".xml";
			String path_bin = _path + ".bin";

			if( this->makeBin_( _pak, path_xml, path_bin ) == false )
			{
				return false;
			}

			file_bin = FileEngine::get()
				->openInputFile( _pak, path_bin );

			done = this->importBin_( file_bin, _archive, reimport );
		}
#	endif

		file_bin->close();

		return done;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::importBin_( FileInputStreamInterface * _bin, Archive & _archive, bool & _reimport )
	{
		int size = _bin->size();

		if( size == 0 )
		{
			_archive.clear();
			return false;
		}


		_archive.resize( size );

		int reading = _bin->read( &_archive.front(), size );

		if( reading != size )
		{
			return false;
		}

		if( _archive[0] != 42 )
		{
			return false;
		}

		//if( *(int*)&_archive[1] != Menge::Protocol::version )
		//{
		//	_reimport = true;
		//	return false;
		//}

		return true;
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::openBin_( const ConstString & _pak, const String & _path, FileInputStreamInterface ** _file, bool & _exist )
	{
		String path_xml = _path + ".xml";
		String path_bin = _path + ".bin";

		if( FileEngine::get()
			->existFile( _pak, path_xml ) == false )
		{
			if( FileEngine::get()
				->existFile( _pak, path_bin ) == false )
			{
				_exist = false;

				return false;
			}

			FileInputStreamInterface * file_bin = FileEngine::get()
				->openInputFile( _pak, path_bin );

			if( file_bin == 0 )
			{
				return false;
			}

			*_file = file_bin;

			return true;
		}

		_exist = true;

		if( FileEngine::get()
			->existFile( _pak, path_bin ) == false )
		{
			if( this->makeBin_( _pak, path_xml, path_bin ) == false )
			{
				*_file = 0;

				return false;
			}
		}

		FileInputStreamInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, path_bin );

		if( file_bin == 0 )
		{
			return false;
		}

		FileInputStreamInterface * file_xml = FileEngine::get()
			->openInputFile( _pak, path_xml );

		if( file_xml == 0 )
		{
			file_bin->close();

			return false;
		}

		time_t time_xml;
		file_xml->time( time_xml );

		time_t time_bin;
		file_bin->time( time_bin );

		file_xml->close();

		if( time_xml > time_bin )
		{
			//Rebild bin file from xml
			file_bin->close();

			if( this->makeBin_( _pak, path_xml, path_bin ) == false )
			{
				*_file = 0;

				return false;
			}

			file_bin = FileEngine::get()
				->openInputFile( _pak, path_bin );
		}

		*_file = file_bin;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::makeBin_( const ConstString & _pak, const String & _pathXml, const String & _pathBin )
	{
		XmlDecoderInterface * decoder = CodecEngine::get()
			->createDecoderT<XmlDecoderInterface>( Consts::get()->c_xml2bin, 0 );

		XmlCodecOptions options;
		options.protocol = "protocol.xml";

		String path;
			
		if( FileEngine::get()
			->getFileSystemPath( _pak, path ) == false )
		{
			options.pathXml = _pathXml;
			options.pathBin = _pathBin;
		}
		else
		{
			options.pathXml = path + "\\" + _pathXml;
			options.pathBin = path + "\\" + _pathBin;
		}

		//options.version = Menge::Protocol::version;

		decoder->setOptions( &options );

		if( decoder->initialize() == false )
		{
			decoder->destroy();

			return false;
		}

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
	bool LoaderEngine::openBin_( const ConstString & _pak, const String & _path, FileInputStreamInterface ** _file, bool & _exist )
	{
		String path_bin = _path + ".bin";
        
		if( FileEngine::get()
			->existFile( _pak, path_bin ) == false )
		{
			_exist = false;

			return false;
		}

		FileInputStreamInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, path_bin );

		if( file_bin == 0 )
		{
			return false;
		}

		*_file = file_bin;

		return true;
	}
#	endif
}