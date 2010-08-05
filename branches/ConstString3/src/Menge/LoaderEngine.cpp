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
	bool LoaderEngine::load( const ConstString & _pak, const String & _path, Loadable * _loadable )
	{
		if( this->import( _pak, _path, m_bufferArchive ) == false )
		{
			return false;
		}

		if( this->loadBinary( m_bufferArchive, _loadable ) == false )
		{
			return false;
		}

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
		String path_bin = _path + ".bin";

		FileInputInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, path_bin );

		String path_xml = _path + ".xml";

		if( file_bin == 0 )
		{
			file_bin = makeBin_( _pak, path_xml, path_bin );
		}
		else
		{
			FileInputInterface * file_xml = FileEngine::get()
				->openInputFile( _pak, path_xml );

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
		}

		if( file_bin == 0 )
		{
			return false;
		}

		int size = file_bin->size();

		if( size == 0 )
		{
			_archive.clear();
			return true;
		}

		_archive.resize( size );

		int reading = file_bin->read( &_archive.front(), size );

		file_bin->close();

		if( reading != size )
		{
			return false;
		}

		return true;
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

		decoder->setOptions( &options );
		
		if( decoder->initialize() == false )
		{
			CodecEngine::get()
				->releaseDecoder( decoder );
			
			return 0;
		}
		
		decoder->decode( 0, 0 );

		CodecEngine::get()
			->releaseDecoder( decoder );

		FileInputInterface * fileBin = FileEngine::get()
			->openInputFile( _pak, _pathBin );

		return fileBin;
	}
}