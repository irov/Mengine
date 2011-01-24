#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "BinParser.h"

#	include "Utils/Core/Stream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_duration(0.f)
		, m_width(0.f)
		, m_height(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::~ResourceMovie()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::setPathMOV( const ConstString & _path )
	{
		m_pathMOV = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceMovie::getPathMOV() const
	{
		return m_pathMOV;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceMovie::getLayerSize() const
	{
		return m_layers.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const MovieLayer & ResourceMovie::getLayer( std::size_t _index ) const
	{
		return m_layers[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loader( BinParser * _parser )
	{
		ResourceReference::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::File_Path, &ResourceMovie::setPathMOV );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		if( m_pathMOV.empty() == true )
		{
			return false;
		}

		if( this->readMOV_( m_pathMOV ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		ResourceReference::_release();

		m_layers.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		template<class C> 
		static bool s_readLayerKey( FileInputInterface * _fs, C & _keys, char _id, int _dementions )
		{
			char id = 0;
			Utils::readStream( _fs, id );

			if( id != _id )
			{
				return false;
			}

			std::size_t size = 0;
			Utils::readStream( _fs, size );

			for( int i = 0; i != size; ++i )
			{
				typename C::value_type key;

				Utils::readStream( _fs, (float *)&key.value, _dementions );
				Utils::readStream( _fs, key.time );
				Utils::readStream( _fs, key.interp );

				_keys.push_back( key );
			}

			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::readMOV_( const ConstString & _file )
	{
		const ConstString & category = this->getCategory();

		FileInputInterface * stream = FileEngine::get()
			->openInputFile( category, _file.str() );

		char magic_number = 0;
		Utils::readStream( stream, magic_number );

		if( magic_number != 42 + 1 )
		{
			stream->close();

			return false;
		}

		Utils::readStreamString( stream, m_name );
		Utils::readStream( stream,  m_duration );
		Utils::readStream( stream,  m_width );
		Utils::readStream( stream,  m_height );

		std::size_t layer_size = 0;
		Utils::readStream( stream, layer_size );

		for( std::size_t i = 0; i != layer_size; ++i )
		{
			MovieLayer layer;

			Utils::readStream( stream, layer.index );
			Utils::readStreamString( stream, layer.name );
			Utils::readStream( stream, layer.width );
			Utils::readStream( stream, layer.height );

			Utils::readStream( stream, layer.in );
			Utils::readStream( stream, layer.out );

			char id_source = 0;
			Utils::readStream( stream, id_source );

			if( id_source != 1 )
			{
				m_layers.clear();
				return false;
			}

			Utils::readStreamString( stream, layer.path );

			Helper::s_readLayerKey( stream, layer.anchorPoints, 2, 2 );
			Helper::s_readLayerKey( stream, layer.positions, 3, 2 );
			Helper::s_readLayerKey( stream, layer.scales, 4, 2 );
			Helper::s_readLayerKey( stream, layer.rotations, 5, 1 );
			Helper::s_readLayerKey( stream, layer.opacities, 6, 1 );

			m_layers.push_back( layer );
		}

		stream->close();

		return true;
	}
}