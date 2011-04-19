#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "BinParser.h"
#	include "Consts.h"

#	include "Utils/Core/Stream.h"
#	include "Logger/Logger.h"

#	include "Math/mat4.h"

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
		return m_layers2D.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const MovieLayer2D & ResourceMovie::getLayer( std::size_t _index ) const
	{
		return m_layers2D[_index];
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
	bool ResourceMovie::getFrame( const MovieLayer2D & _layer, float _timing, MovieFrame2D & _frame ) const
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
	
		const MovieFrame2D & frame_1 = _layer.frames[index+0];
		const MovieFrame2D & frame_2 = _layer.frames[index+1];

		float time_1 = (index + 0) * m_duration;
		float time_2 = (index + 1) * m_duration;

		float timeScale = ( relation_time - time_1 ) / ( time_2 - time_1 );

		_frame.anchorPoint = Helper::s_linerp_v2( frame_1.anchorPoint, frame_2.anchorPoint, timeScale );
		_frame.position = Helper::s_linerp_v2( frame_1.position, frame_2.position, timeScale );
		_frame.scale = Helper::s_linerp_v2( frame_1.scale, frame_2.scale, timeScale );
		_frame.angle = Helper::s_linerp( frame_1.angle, frame_2.angle, timeScale );
		_frame.opacity = Helper::s_linerp( frame_1.opacity, frame_2.opacity, timeScale );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameFirst( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	{
		if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.front();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameLast( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	{
		if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.back();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getMovieInternal( const ConstString & _source, MovieInternal & _internal ) const
	{
		TMapInternals::const_iterator it_found = m_internals.find(_source);

		if( it_found == m_internals.end() )
		{
			return false;
		}

		_internal = it_found->second;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loader( BinParser * _parser )
	{
		ResourceReference::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE( Protocol::Duration_Value, m_duration );

			BIN_CASE_NODE( Protocol::MovieLayer2D )
			{
				m_layers2D.push_back( MovieLayer2D() );
				MovieLayer2D & ml = m_layers2D.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Name, ml.name );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Parent, ml.parent );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Source, ml.source);
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Internal, ml.internal );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Out, ml.out );
				}

				BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer2D_, ml );
			}

			BIN_CASE_NODE( Protocol::MovieLayer3D )
			{
				m_layers3D.push_back( MovieLayer3D() );
				MovieLayer3D & ml = m_layers3D.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Name, ml.name );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Parent, ml.parent );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Source, ml.source);
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Internal, ml.internal );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Out, ml.out );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraPosition, ml.cameraPosition );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraInterest, ml.cameraInterest );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraFOV, ml.cameraFOV );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Width, ml.width );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Height, ml.height );
				}

				BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer3D_, ml );
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

				MovieFootage fg;
				fg.path = path;

				m_imagePaths.insert( std::make_pair( name, fg) );
			}

			BIN_CASE_NODE( Protocol::Internal )
			{
				ConstString name;
				ConstString group;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Internal_Name, name );
					BIN_CASE_ATTRIBUTE( Protocol::Internal_Group, group );
				}

				MovieInternal il;
				il.group = group;

				m_internals.insert( std::make_pair( name, il) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loaderMovieLayer2D_( BinParser * _parser, MovieLayer2D & _ml )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame2D )
			{
				MovieFrame2D frame;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Scale, frame.scale );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Rotation, frame.angle );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Opacity, frame.opacity );
				}

				_ml.frames.push_back( frame );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loaderMovieLayer3D_( BinParser * _parser, MovieLayer3D & _ml )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame3D )
			{
				MovieFrame3D frame;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Scale, frame.scale );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Rotation, frame.rotation );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Opacity, frame.opacity );
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
			movieImageResource += Helper::to_str(it->first);

			ConstString c_movieImageResource(movieImageResource);

			if( ResourceManager::get()->hasResource( c_movieImageResource ) == false )
			{
				ResourceImageDefault * image_resource = ResourceManager::get()
					->createResourceT<ResourceImageDefault>( m_category, m_group, c_movieImageResource, Consts::get()->c_ResourceImageDefault );

				image_resource->addImagePath( it->second.path, mt::vec2f(-1.f,-1.f) );
			}
		}

		for( TVectorMovieLayers2D::iterator
			it = m_layers2D.begin(),
			it_end = m_layers2D.end();
		it != it_end;
		++it )
		{
			if( it->internal == true )
			{
				continue;
			}

			TMapImagePaths::iterator it_found = m_imagePaths.find( it->source );

			if( it_found == m_imagePaths.end() )
			{
				MENGE_LOG_ERROR("ResourceMovie: '%s' can't find image '%s'"
					, m_name.c_str()
					, it->source.c_str()
					);

				return false;
			}
		}

		for( TVectorMovieLayers3D::iterator
			it = m_layers3D.begin(),
			it_end = m_layers3D.end();
		it != it_end;
		++it )
		{
			MovieLayer3D & layer = *it;

			this->compileModel_( layer );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::compileModel_( MovieLayer3D & _layer )
	{
		mt::vec3f axis_x(1.f,0.f,0.f);
		mt::vec3f axis_y(0.f,1.f,0.f);
		mt::vec3f axis_z(0.f,0.f,1.f); 

		for( MovieLayer3D::TVectorFrames::iterator
			it = _layer.frames.begin(),
			it_end = _layer.frames.end();
		it != it_end;
		++it )
		{
			MovieFrame3D & frame = *it;

			mt::vec3f point1 = axis_x * _layer.width * 0.f + axis_z * _layer.height * 0.f;
			mt::vec3f point2 = axis_x * _layer.width * 1.f + axis_z * _layer.height * 0.f;
			mt::vec3f point3 = axis_x * _layer.width * 1.f + axis_z * _layer.height * 1.f;
			mt::vec3f point4 = axis_x * _layer.width * 0.f + axis_z * _layer.height * 1.f;

			mt::mat4f x_axis_m4;
			mt::make_rotate_x_axis_m4( x_axis_m4, frame.rotation.x );
			
			mt::mat4f y_axis_m4;
			mt::make_rotate_x_axis_m4( y_axis_m4, frame.rotation.y );
			
			mt::mat4f z_axis_m4;
			mt::make_rotate_x_axis_m4( z_axis_m4, frame.rotation.z );

			mt::mat4f scale_m4;
			mt::make_scale_m4( scale_m4, frame.scale.x, 1.f, frame.scale.y );

			mt::mat4f translation_m4;
			mt::make_translation_m4( translation_m4, frame.position.x, frame.position.y, frame.position.z );

			mt::mat4f worldmatrix_m4_1;
			mt::ident_m4(worldmatrix_m4_1);

			mt::mat4f worldmatrix_m4_2;
			mt::mul_m4_m4( worldmatrix_m4_2, worldmatrix_m4_1, x_axis_m4 );

			mt::mat4f worldmatrix_m4_3;
			mt::mul_m4_m4( worldmatrix_m4_3, worldmatrix_m4_2, y_axis_m4 );

			mt::mat4f worldmatrix_m4_4;
			mt::mul_m4_m4( worldmatrix_m4_4, worldmatrix_m4_3, z_axis_m4 );

			mt::mat4f worldmatrix_m4_5;
			mt::mul_m4_m4( worldmatrix_m4_5, worldmatrix_m4_4, scale_m4 );

			mt::mat4f worldmatrix_m4_6;
			mt::mul_m4_m4( worldmatrix_m4_6, worldmatrix_m4_5, translation_m4 );


		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		ResourceReference::_release();
	}
}