#	include "LoaderEngine.h"
#	include "Loadable.h"

#	include "BinParser.h"
#	include "FileEngine.h"

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
		FileInputInterface * file = FileEngine::get()
			->openInputFile( _pak, _path );

		if( file == 0 )
		{
			return false;
		}

		int size = file->size();

		m_bufferArchive.resize( size );
		file->read( &m_bufferArchive[0], size );

		if( this->loadBinary( m_bufferArchive, _loadable ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::loadBinary( const Archive & _blob, Loadable * _loadable )
	{
		BinParser parser( _blob );

		LoadableListener listener(_loadable);
		if( parser.run( &listener ) == false )
		{
			return false;
		}

		_loadable->_loaded();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool LoaderEngine::import( const ConstString & _pak, const String & _path, Archive & _archive )
	{
		String xml_path = _path + ".xml";

		FileInputInterface * file = FileEngine::get()
			->openInputFile( _pak, xml_path );

		
	}
}