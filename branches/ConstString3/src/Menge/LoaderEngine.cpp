#	include "LoaderEngine.h"
#	include "Loadable.h"

#	include "BinParser.h"
#	include "FileEngine.h"

#	include "CodecEngine.h"
#	include "Interface/XmlCodecInterface.h"

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
	bool LoaderEngine::load( const ConstString & _pak, const String & _path, Loadable * _loadable )
	{
		Archive & buffer = m_bufferArchive[m_bufferLevel];

		++m_bufferLevel;

		if( this->import( _pak, _path, buffer ) == false )
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

		_loadable->loaded();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::import( const ConstString & _pak, const String & _path, Archive & _archive )
	{
		FileInputInterface * file_bin = this->openBin_( _pak, _path, false );

		if( file_bin == 0 )
		{
			return false;
		}

		bool reimport = false;
		bool done = this->importBin_( file_bin, _archive, reimport );

		if( reimport == true )
		{
			file_bin->close();

			file_bin = this->openBin_( _pak, _path, true );

			done = this->importBin_( file_bin, _archive, reimport );
		}

		file_bin->close();

		return done;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::importBin_( FileInputInterface * _bin, Archive & _archive, bool & _reimport )
	{
		int size = _bin->size();

		if( size == 0 )
		{
			_archive.clear();
			return true;
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

		if( *(int*)&_archive[1] != Menge::Protocol::version )
		{
			_reimport = true;
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface * LoaderEngine::openBin_( const ConstString & _pak, const String & _path, bool _force )
	{
		String path_xml = _path + ".xml";
		String path_bin = _path + ".bin";

		if( FileEngine::get()
			->existFile( _pak, path_bin ) == false || _force )
		{
			FileInputInterface * file_bin = makeBin_( _pak, path_xml, path_bin );

			return file_bin;
		}

		FileInputInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, path_bin );

		if( file_bin == 0 )
		{
			return false;
		}

		FileInputInterface * file_xml = FileEngine::get()
			->openInputFile( _pak, path_xml );

		if( file_xml == 0 )
		{
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

			file_bin = makeBin_( _pak, path_xml, path_bin );
		}

		return file_bin;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface * LoaderEngine::makeBin_( const ConstString & _pak, const String & _pathXml, const String & _pathBin )
	{
		XmlDecoderInterface * decoder = CodecEngine::get()
			->createDecoderT<XmlDecoderInterface>( "xml2bin", 0 );

		XmlCodecOptions options;
		options.protocol = "protocol.xml";

		const String & path = FileEngine::get()
			->getFileSystemPath( _pak );

		if( path.empty() )
		{
			options.pathXml = _pathXml;
			options.pathBin = _pathBin;
		}
		else
		{
			options.pathXml = path + "\\" + _pathXml;
			options.pathBin = path + "\\" + _pathBin;
		}

		options.version = Menge::Protocol::version;

		decoder->setOptions( &options );
		
		if( decoder->initialize() == false )
		{
			CodecEngine::get()
				->releaseDecoder( decoder );
			
			return 0;
		}
		
		if( decoder->decode( 0, 0 ) == 0 )
		{
			CodecEngine::get()
				->releaseDecoder( decoder );

			return 0;
		}

		CodecEngine::get()
			->releaseDecoder( decoder );

		FileInputInterface * fileBin = FileEngine::get()
			->openInputFile( _pak, _pathBin );

		return fileBin;
	}
}