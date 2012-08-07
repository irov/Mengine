#	include "LoaderEngine.h"

#	include "metabuf/Metabuf.hpp"

#	include "FileEngine.h"

#	include "CodecEngine.h"
#	include "Interface/XmlCodecInterface.h"

#	include "Consts.h"

#   include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoaderEngine::LoaderEngine()
		: m_bufferLevel(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::load( const ConstString & _pak, const WString & _path, Metabuf::Metadata * _metadata, bool & _exist )
	{
        MENGE_LOG_INFO( "LoaderEngine::load pak '%s:%S'"
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

        
		if( this->loadBinary( buffer, _metadata ) == false )
		{
			--m_bufferLevel;
			return false;
		}

		--m_bufferLevel;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::loadBinary( const Archive & _blob, Metabuf::Metadata * _metadata )
	{
        if( _blob.empty() == true )
        {
            return true;
        }

		size_t read_size;
		if( _metadata->parse( &_blob[0], _blob.size(), read_size ) == false )
        {
            MENGE_LOG_ERROR("LoaderEngine::loadBinary invlid parse"
                );

            return false;
        }

        if( read_size != _blob.size() )
        {
            return false;
        }
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::import( const ConstString & _pak, const WString & _path, Archive & _archive, bool & _exist )
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

			WString path_xml = _path + L".xml";
			WString path_bin = _path + L".bin";

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
			return true;
		}

		_archive.resize( size );

		int reading = _bin->read( &_archive.front(), size );

		if( reading != size )
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
	bool LoaderEngine::openBin_( const ConstString & _pak, const WString & _path, FileInputStreamInterface ** _file, bool & _exist )
	{
		WString path_xml = _path;

        WString::size_type size = path_xml.size();
        path_xml[size-3] = L'x';
        path_xml[size-2] = L'm';
        path_xml[size-1] = L'l';

		if( FileEngine::get()
			->existFile( _pak, path_xml ) == false )
		{
			if( FileEngine::get()
				->existFile( _pak, _path ) == false )
			{
				_exist = false;

				return false;
			}

			FileInputStreamInterface * file_bin = FileEngine::get()
				->openInputFile( _pak, _path );

			if( file_bin == 0 )
			{
				return false;
			}

			*_file = file_bin;

			return true;
		}

		_exist = true;

		if( FileEngine::get()
			->existFile( _pak, _path ) == false )
		{
			if( this->makeBin_( _pak, path_xml, _path ) == false )
			{
				*_file = 0;

				return false;
			}
		}

		FileInputStreamInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, _path );

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

			if( this->makeBin_( _pak, path_xml, _path ) == false )
			{
				*_file = 0;

				return false;
			}

			file_bin = FileEngine::get()
				->openInputFile( _pak, _path );
		}

		*_file = file_bin;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::makeBin_( const ConstString & _pak, const WString & _pathXml, const WString & _pathBin )
	{
		XmlDecoderInterface * decoder = CodecEngine::get()
			->createDecoderT<XmlDecoderInterface>( Consts::get()->c_xml2bin, 0 );

		if( decoder == NULL )
		{
			MENGE_LOG_ERROR("LoaderEngine::makeBin_ invalid create decoder xml2bin for %s:%S"
				, _pak.c_str()
				, _pathXml.c_str()
				);

			return false;
		}

		XmlCodecOptions options;
		options.pathProtocol = L"protocol.xml";

		WString path;
			
		if( FileEngine::get()
			->getFileSystemPath( _pak, path ) == false )
		{
			options.pathXml = _pathXml;
			options.pathBin = _pathBin;
		}
		else
		{
			options.pathXml = path + _pathXml;
			options.pathBin = path + _pathBin;
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
	bool LoaderEngine::openBin_( const ConstString & _pak, const WString & _path, FileInputStreamInterface ** _file, bool & _exist )
	{
		if( FileEngine::get()
			->existFile( _pak, _path ) == false )
		{
			_exist = false;

			return false;
		}

		FileInputStreamInterface * file_bin = FileEngine::get()
			->openInputFile( _pak, _path );

		if( file_bin == 0 )
		{
			return false;
		}

		*_file = file_bin;

		return true;
	}
#	endif
}