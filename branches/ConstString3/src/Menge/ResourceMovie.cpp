#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "BinParser.h"
#	include "Consts.h"

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
	namespace Helper
	{
		static float s_linerp( float _in, float _out, float _scale )
		{
			return _in + ( _out - _in ) * _scale; 
		}

		static mt::vec2f s_linerp_v2( const mt::vec2f & _in, const mt::vec2f & _out, float _scale )
		{
			float x = s_linerp(_in.x, _out.x, _scale);
			float y = s_linerp(_in.y, _out.y, _scale);
			
			return mt::vec2f(x, y);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame( const MovieLayer & _layer, float _timing, MovieLayer::Frame & _frame ) const
	{
		if( _timing < _layer.in )
		{
			return false;
		}

		if( _timing >= _layer.out )
		{
			return false;
		}

		float relation_time = _timing - _layer.in;

		std::size_t index = std::size_t(relation_time / m_duration);
	
		const MovieLayer::Frame & frame_1 = _layer.frames[index+0];
		const MovieLayer::Frame & frame_2 = _layer.frames[index+1];

		float time_1 = (index + 0) * m_duration;
		float time_2 = (index + 1) * m_duration;

		float timeScale = ( relation_time - time_1 ) / ( time_2 - time_1 );

		_frame.anchorPoint = Helper::s_linerp_v2( frame_1.anchorPoint, frame_2.anchorPoint, timeScale );
		_frame.position = Helper::s_linerp_v2( frame_1.position, frame_2.position, timeScale );
		_frame.scale = Helper::s_linerp_v2( frame_1.scale, frame_2.scale, timeScale );
		_frame.angle = Helper::s_linerp( frame_1.angle, frame_2.angle, timeScale );
		_frame.opacity = Helper::s_linerp( frame_1.opacity, frame_2.opacity, timeScale );

		//_frame = frame_1;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameFirst( const MovieLayer & _layer, MovieLayer::Frame & _frame ) const
	{
		if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.front();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameLast( const MovieLayer & _layer, MovieLayer::Frame & _frame ) const
	{
		if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.back();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loader( BinParser * _parser )
	{
		ResourceReference::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Duration_Value, m_duration );

			BIN_CASE_NODE( Protocol::MovieLayer )
			{
				m_layers.push_back( MovieLayer() );
				MovieLayer & ml = m_layers.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Name, ml.name )
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Source, ml.source)
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Out, ml.out );
				}

				BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer_, ml );
			}

			BIN_CASE_NODE( Protocol::File )
			{
				ConstString name;
				ConstString path;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::File_Name, name );
					BIN_CASE_ATTRIBUTE( Protocol::File_Path, path );
				}

				Footage fg;
				fg.path = path;

				m_imagePaths.insert( std::make_pair( name, fg) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loaderMovieLayer_( BinParser * _parser, MovieLayer & _ml )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame )
			{
				MovieLayer::Frame frame;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Scale, frame.scale );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Rotation, frame.angle );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Opacity, frame.opacity );
				}

				_ml.frames.push_back( frame );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		for( TMapImagePaths::iterator
			it = m_imagePaths.begin(),
			it_end = m_imagePaths.end();
		it != it_end;
		++it )
		{
			String movieImageResource = "MovieLayerImage";
			movieImageResource += it->first.str();

			if( ResourceManager::get()->hasResource( movieImageResource ) == false )
			{
				ResourceImageDefault * image_resource = ResourceManager::get()
					->createResourceT<ResourceImageDefault>( movieImageResource, Consts::get()->c_ResourceImageDefault );

				ResourceManager::get()
					->registerResource( image_resource );

				image_resource->addImagePath( it->second.path, mt::vec2f(-1.f,-1.f) );
			}
		}

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			TMapImagePaths::iterator it_found = m_imagePaths.find( it->source );

			if( it_found == m_imagePaths.end() )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		ResourceReference::_release();

		m_layers.clear();
	}
}