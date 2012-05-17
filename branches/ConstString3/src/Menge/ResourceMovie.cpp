#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "ResourceImageDefault.h"

#	include "BinParser.h"
#	include "Consts.h"

#	include "Utils/Core/Stream.h"
#	include "LogEngine.h"

#	include "Math/mat4.h"

#	include "MovieFramePack.h"
#	include "MovieKeyFrameManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_frameDuration(0.f)
		, m_width(0.f)
		, m_height(0.f)
		, m_framePack(NULL)
		, m_maxLayerIndex(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::~ResourceMovie()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceMovie::getFrameDuration() const
	{
		return m_frameDuration;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceMovie::getWorkAreaDuration() const
	{
		return m_workAreaDuration;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceMovie::getFrameCount() const
	{
		return m_frameCount;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceMovie::getMaxLayerIndex() const
	{
		return m_maxLayerIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorMovieLayers2D & ResourceMovie::getLayers2D() const
	{
		return m_layers2D;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorMovieLayers3D & ResourceMovie::getLayers3D() const
	{
		return m_layers3D;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		static void s_linerp( float & _out, float _in1, float _in2, float _scale )
		{
			_out = _in1 + ( _in2 - _in1 ) * _scale; 
		}

		static void s_linerp_v2( mt::vec2f & _out, const mt::vec2f & _in1, const mt::vec2f & _in2, float _scale )
		{
			s_linerp(_out.x, _in1.x, _in2.x, _scale);
			s_linerp(_out.y, _in1.y, _in2.y, _scale);
		}

		static void s_linerp_f4( float * _out, const float * _frame1, const float * _frame2, float _scale )
		{
			s_linerp(_out[0], _frame1[0], _frame2[0], _scale);
			s_linerp(_out[1], _frame1[1], _frame2[1], _scale);
			s_linerp(_out[2], _frame1[2], _frame2[2], _scale);
			s_linerp(_out[3], _frame1[3], _frame2[3], _scale);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame2D( const MovieLayer2D & _layer, size_t _index, MovieFrame2D & _frame ) const
	{
		//if( _timing < _layer.in )
		//{
		//	return false;
		//}

		//if( _timing >= _layer.out )
		//{
		//	return false;
		//}

		if( m_framePack->isLayerEmpty( _layer.index ) == true )
		{
			return false;
		}

		const MovieFrameSource & frame =  m_framePack->getLayerFrame( _layer.index, _index );

		_frame.anchorPoint = frame.anchorPoint;
		_frame.position = frame.position;
		_frame.scale = frame.scale;
		_frame.angle = frame.rotation.z;
		_frame.opacity = frame.opacity;

		/*const MovieFrame2D & frame = _layer.frames[_index];

		_frame.anchorPoint = frame.anchorPoint;
		_frame.position = frame.position;
		_frame.scale = frame.scale;
		_frame.angle = frame.angle;
		_frame.opacity = frame.opacity;*/

		//float relation_time = _timing - _layer.in;

		//size_t index = size_t(relation_time / m_frameDuration);

		//const MovieFrame2D & frame_1 = _layer.frames[index+0];
		//const MovieFrame2D & frame_2 = _layer.frames[index+1];

		//float time_1 = (index + 0) * m_frameDuration;
		//float time_2 = (index + 1) * m_frameDuration;

		//float timeScale = ( relation_time - time_1 ) / ( time_2 - time_1 );

		//Helper::s_linerp_v2( _frame.anchorPoint, frame_1.anchorPoint, frame_2.anchorPoint, timeScale );
		//Helper::s_linerp_v2( _frame.position, frame_1.position, frame_2.position, timeScale );
		//Helper::s_linerp_v2( _frame.scale, frame_1.scale, frame_2.scale, timeScale );
		//Helper::s_linerp( _frame.angle, frame_1.angle, frame_2.angle, timeScale );
		//Helper::s_linerp( _frame.opacity, frame_1.opacity, frame_2.opacity, timeScale );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//bool ResourceMovie::getFrame2DFirst( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	//{
	//	if( _layer.frames.empty() == true )
	//	{
	//		return false;
	//	}

	//	_frame = _layer.frames.front();

	//	return true;
	//}
	////////////////////////////////////////////////////////////////////////////
	//bool ResourceMovie::getFrame2DLast( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	//{
	//	if( _layer.frames.empty() == true )
	//	{
	//		return false;
	//	}

	//	_frame = _layer.frames.back();
	//	//_frame = _layer.frames[_layer.frames.size() - 2];

	//	return true;
	//}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame2DFirst( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	{
		if( m_framePack->isLayerEmpty( _layer.index ) == true )
		{
			return false;
		}

		this->getFrame2D( _layer , 0, _frame );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame2DLast( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const
	{
		if( m_framePack->isLayerEmpty( _layer.index ) == true )
		{
			return false;
		}

		size_t indexLast = m_framePack->getLayerCountFrames(_layer.index ) - 1;

		bool isGet = this->getFrame2D( _layer , indexLast, _frame );
		return isGet;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame3D( const MovieLayer3D & _layer, float _timing, MovieFrame3D & _frame ) const
	{
		//if( _timing < _layer.in )
		//{
		//	return false;
		//}

		//if( _timing >= _layer.out )
		//{
		//	return false;
		//}

		//float relation_time = _timing - _layer.in;

		//size_t index = size_t(relation_time / m_frameDuration);

		//const MovieFrame3D & frame_1 = _layer.frames[index+0];
		//const MovieFrame3D & frame_2 = _layer.frames[index+1];

		//float time_1 = (index + 0) * m_frameDuration;
		//float time_2 = (index + 1) * m_frameDuration;

		//float timeScale = ( relation_time - time_1 ) / ( time_2 - time_1 );

		//Helper::s_linerp_f4( _frame.vertices[0].pos, frame_1.vertices[0].pos, frame_2.vertices[0].pos, timeScale );
		//Helper::s_linerp_f4( _frame.vertices[1].pos, frame_1.vertices[1].pos, frame_2.vertices[1].pos, timeScale );
		//Helper::s_linerp_f4( _frame.vertices[2].pos, frame_1.vertices[2].pos, frame_2.vertices[2].pos, timeScale );
		//Helper::s_linerp_f4( _frame.vertices[3].pos, frame_1.vertices[3].pos, frame_2.vertices[3].pos, timeScale );

		//_frame.vertices[0].color = frame_1.vertices[0].color;
		//_frame.vertices[0].uv[0] = frame_1.vertices[0].uv[0];
		//_frame.vertices[0].uv[1] = frame_1.vertices[0].uv[1];

		//_frame.vertices[1].color = frame_1.vertices[1].color;
		//_frame.vertices[1].uv[0] = frame_1.vertices[1].uv[0];
		//_frame.vertices[1].uv[1] = frame_1.vertices[1].uv[1];

		//_frame.vertices[2].color = frame_1.vertices[2].color;
		//_frame.vertices[2].uv[0] = frame_1.vertices[2].uv[0];
		//_frame.vertices[2].uv[1] = frame_1.vertices[2].uv[1];

		//_frame.vertices[3].color = frame_1.vertices[3].color;
		//_frame.vertices[3].uv[0] = frame_1.vertices[3].uv[0];
		//_frame.vertices[3].uv[1] = frame_1.vertices[3].uv[1];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame3DFirst( const MovieLayer3D & _layer, MovieFrame3D & _frame ) const
	{
		/*if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.front();

		return true;*/
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame3DLast( const MovieLayer3D & _layer, MovieFrame3D & _frame ) const
	{
		/*if( _layer.frames.empty() == true )
		{
			return false;
		}

		_frame = _layer.frames.back();

		return true;*/
		return false;
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
			//BIN_CASE_ATTRIBUTE( Protocol::Duration_Value, m_duration );
			BIN_CASE_ATTRIBUTE( Protocol::FrameDuration_Value, m_frameDuration );
			BIN_CASE_ATTRIBUTE( Protocol::WorkAreaDuration_Value, m_workAreaDuration );

			BIN_CASE_ATTRIBUTE( Protocol::Width_Value, m_width );
			BIN_CASE_ATTRIBUTE( Protocol::Height_Value, m_height );
			BIN_CASE_ATTRIBUTE( Protocol::KeyFramesPackPath_Path, m_keyFramePackPath );
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
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Out, ml.out );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_BlendingMode, ml.blendingMode );
				}

				if( ml.in < 0.0f )
				{
					ml.in = 0.0f;
				}

				//ml.out -= m_frameDuration;

				//BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer2D_, ml );
			}

			BIN_CASE_NODE( Protocol::MovieLayer3D )
			{
				m_layers3D.push_back( MovieLayer3D() );
				MovieLayer3D & ml = m_layers3D.back();

				MovieLayerSource3D layer;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Name, ml.name );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Parent, ml.parent );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Source, ml.source);
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Internal, ml.internal );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Out, ml.out );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_BlendingMode, ml.blendingMode );

					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraPosition, layer.cameraPosition );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraInterest, layer.cameraInterest );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraFOV, layer.cameraFOV );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_CameraAspect, layer.cameraAspect );

					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Width, layer.width );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Height, layer.height );
				}

				mt::mat4f view;
				mt::make_lookat_m4( view, layer.cameraPosition, layer.cameraInterest );

				mt::mat4f projection;
				mt::make_projection_m4( projection, layer.cameraFOV, layer.cameraAspect, 1.f, 10000.f );

				mt::mul_m4_m4( layer.vp, view, projection );

				//BIN_PARSE_METHOD_ARG2( this, &ResourceMovie::loaderMovieLayer3D_, ml, layer );
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////
	//void ResourceMovie::loaderMovieLayer2D_( BinParser * _parser, MovieLayer2D & _ml )
	//{
	//	BIN_SWITCH_ID(_parser)
	//	{
	//		BIN_CASE_NODE( Protocol::KeyFrame2D )
	//		{
	//			MovieFrame2D frame;				

	//			if( _ml.frames.empty() == false )
	//			{
	//				const MovieFrame2D & old_frame = _ml.frames.back();

	//				frame.scale = old_frame.scale;
	//				frame.position = old_frame.position;
	//				frame.angle = old_frame.angle;
	//				frame.opacity = old_frame.opacity;
	//				frame.anchorPoint = old_frame.anchorPoint;
	//			}

	//			size_t count = 1;

	//			mt::vec2f anchorPoint2d = frame.anchorPoint.to_vec2f();
	//			mt::vec2f position2d = frame.position.to_vec2f();
	//			mt::vec2f scale2d = frame.scale.to_vec2f();

	//			BIN_FOR_EACH_ATTRIBUTES()
	//			{
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_AnchorPoint, anchorPoint2d );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Position, position2d );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Scale, scale2d );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Rotation, frame.angle );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Opacity, frame.opacity );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Index, count );
	//			}

	//			frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
	//			frame.position = mt::vec3f(position2d, 0.f);
	//			frame.scale = mt::vec3f(scale2d, 1.f);


	//			for( size_t i = 0; i != count; ++i )
	//			{
	//				_ml.frames.push_back(frame);
	//			}
	//		}
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void ResourceMovie::loaderMovieLayer3D_( BinParser * _parser, MovieLayer3D & _ml, const MovieLayerSource3D & _layer )
	//{
	//	BIN_SWITCH_ID(_parser)
	//	{
	//		BIN_CASE_NODE( Protocol::KeyFrame3D )
	//		{
	//			MovieFrameSource3D source;

	//			BIN_FOR_EACH_ATTRIBUTES()
	//			{
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_AnchorPoint, source.anchorPoint );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Position, source.position );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Rotation, source.rotation );
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Scale, source.scale );					
	//				BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Opacity, source.opacity );
	//			}

	//			MovieFrame3D frame;

	//			this->convertSourceToFrame3D_( frame, _layer, source );
	//			_ml.frames.push_back( frame );
	//		}
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		if( m_frameDuration == 0.f )
		{
			return false;
		}

		if( m_keyFramePackPath.empty() == true )
		{
			MENGE_LOG_ERROR("ResourceMovie: '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				);

			return false;
		}

		const ConstString& category = this->getCategory();

		m_framePack = MovieKeyFrameManager::get()
			->getMovieFramePak( category, m_keyFramePackPath );

		if ( m_framePack == NULL )
		{
			MENGE_LOG_ERROR("ResourceMovie: '%s' can` t get frame pack"
				, this->getName().c_str()
				);

			return false;
		}

		for( TVectorMovieLayers2D::iterator
			it = m_layers2D.begin(),
			it_end = m_layers2D.end();
		it != it_end;
		++it )
		{
			m_maxLayerIndex = std::max( m_maxLayerIndex, it->index );

			if( it->source ==  Consts::get()->c_MovieSlot )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;

				continue;
			}
			
			if( it->source ==  Consts::get()->c_MovieNullObject )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;

				continue;
			}

			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( it->source );

			if( resourceType == Consts::get()->c_ResourceImageDefault )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageSolid )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageInAtlas )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceVideo )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceSound )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceMovie )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = true;
			}
			else if( resourceType == Consts::get()->c_ResourceInternalObject )
			{
				it->internal = true;
				it->animatable = false;
				it->movie = false;
			}
			else
			{
				MENGE_LOG_ERROR("ResourceMovie: '%s' can't setup layer2d '%s' type '%s'"
					, this->getName().c_str()
					, it->source.c_str()
					, resourceType.c_str()
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
			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( it->source );

			if( resourceType == Consts::get()->c_ResourceImageDefault )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageInAtlas )
			{
				it->internal = false;
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceVideo )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceSound )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceMovie )
			{
				it->internal = false;
				it->animatable = true;
				it->movie = true;
			}
			else if( resourceType == Consts::get()->c_ResourceInternalObject )
			{
				it->internal = true;
				it->animatable = false;
				it->movie = false;
			}
			else
			{
				MENGE_LOG_ERROR("ResourceMovie: '%s' can't setup layer3d '%s' type '%s'"
					, this->getName().c_str()
					, it->source.c_str()
					, resourceType.c_str()
					);

				return false;
			}
		}

		m_frameCount = (size_t)((m_workAreaDuration / m_frameDuration) + 0.5f) - 1;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::convertSourceToFrame3D_( MovieFrame3D & _frame, const MovieLayerSource3D & _layer, const MovieFrameSource3D & _source )
	{
		//mt::vec3f axis_x(1.f,0.f,0.f);
		//mt::vec3f axis_y(0.f,1.f,0.f);

		//mt::vec3f point0 = axis_x * _layer.width * 0.f + axis_y * _layer.height * 0.f;
		//mt::vec3f point1 = axis_x * _layer.width * 1.f + axis_y * _layer.height * 0.f;
		//mt::vec3f point2 = axis_x * _layer.width * 1.f + axis_y * _layer.height * 1.f;
		//mt::vec3f point3 = axis_x * _layer.width * 0.f + axis_y * _layer.height * 1.f;

		//mt::mat4f anchor_m4;
		//mt::make_translation_m4( anchor_m4, -_source.anchorPoint.x, -_source.anchorPoint.y, -_source.anchorPoint.z );

		//mt::mat4f rotate_m4;
		//mt::make_rotate_m4( rotate_m4, -_source.rotation.z, -_source.rotation.y, -_source.rotation.x );

		//mt::mat4f scale_m4;
		//mt::make_scale_m4( scale_m4, _source.scale.x, _source.scale.y, 1.f );

		//mt::mat4f translation_m4;
		//mt::make_translation_m4( translation_m4, _source.position.x, _source.position.y, _source.position.z );

		//mt::vec3f p00;
		//mt::mul_v3_m4( p00, point0, anchor_m4 );
		//mt::vec3f p01;
		//mt::mul_v3_m4( p01, p00, scale_m4 );
		//mt::vec3f p02;
		//mt::mul_v3_m4( p02, p01, rotate_m4 );
		//mt::vec3f p05;
		//mt::mul_v3_m4( p05, p02, translation_m4 );

		//mt::vec3f p0;
		//float w0 = mt::mul_v3_m4_proj( p0, p05, _layer.vp );

		//p0.x = ( 1.0f + p0.x ) * m_width * 0.5f; 
		//p0.y = ( 1.0f + p0.y ) * m_height * 0.5f;

		//mt::vec3f p10;
		//mt::mul_v3_m4( p10, point1, anchor_m4 );
		//mt::vec3f p11;
		//mt::mul_v3_m4( p11, p10, scale_m4 );
		//mt::vec3f p12;
		//mt::mul_v3_m4( p12, p11, rotate_m4 );
		//mt::vec3f p15;
		//mt::mul_v3_m4( p15, p12, translation_m4 );

		//mt::vec3f p1;
		//float w1 = mt::mul_v3_m4_proj( p1, p15, _layer.vp );

		//p1.x = ( 1.0f + p1.x ) * m_width * 0.5f; 
		//p1.y = ( 1.0f + p1.y ) * m_height * 0.5f;

		//mt::vec3f p20;
		//mt::mul_v3_m4( p20, point2, anchor_m4 );
		//mt::vec3f p21;
		//mt::mul_v3_m4( p21, p20, scale_m4 );
		//mt::vec3f p22;
		//mt::mul_v3_m4( p22, p21, rotate_m4 );
		//mt::vec3f p25;
		//mt::mul_v3_m4( p25, p22, translation_m4 );

		//mt::vec3f p2;
		//float w2 = mt::mul_v3_m4_proj( p2, p25, _layer.vp );

		//p2.x = ( 1.0f + p2.x ) * m_width * 0.5f; 
		//p2.y = ( 1.0f + p2.y ) * m_height * 0.5f;

		//mt::vec3f p30;
		//mt::mul_v3_m4( p30, point3, anchor_m4 );
		//mt::vec3f p31;
		//mt::mul_v3_m4( p31, p30, scale_m4 );
		//mt::vec3f p32;
		//mt::mul_v3_m4( p32, p31, rotate_m4 );
		//mt::vec3f p35;
		//mt::mul_v3_m4( p35, p32, translation_m4 );

		//mt::vec3f p3;
		//float w3 = mt::mul_v3_m4_proj( p3, p35, _layer.vp );

		//p3.x = ( 1.0f + p3.x ) * m_width * 0.5f; 
		//p3.y = ( 1.0f + p3.y ) * m_height * 0.5f;

		//ColourValue colour(1.f, 1.f, 1.f, _source.opacity);
		//uint32 color = colour.getAsRGBA();

		//_frame.vertices[0].pos[0] = p0.x;
		//_frame.vertices[0].pos[1] = p0.y;
		//_frame.vertices[0].pos[2] = 0.f;
		//_frame.vertices[0].pos[3] = w0;

		//_frame.vertices[0].uv[0] = 0.f;
		//_frame.vertices[0].uv[1] = 0.f;

		//_frame.vertices[0].color = color;


		//_frame.vertices[1].pos[0] = p1.x;
		//_frame.vertices[1].pos[1] = p1.y;
		//_frame.vertices[1].pos[2] = 0.f;
		//_frame.vertices[1].pos[3] = w1;

		//_frame.vertices[1].uv[0] = 1.f;
		//_frame.vertices[1].uv[1] = 0.f;

		//_frame.vertices[1].color = color;


		//_frame.vertices[2].pos[0] = p2.x;
		//_frame.vertices[2].pos[1] = p2.y;
		//_frame.vertices[2].pos[2] = 0.f;
		//_frame.vertices[2].pos[3] = w2;

		//_frame.vertices[2].uv[0] = 1.f;
		//_frame.vertices[2].uv[1] = 1.f;

		//_frame.vertices[2].color = color;


		//_frame.vertices[3].pos[0] = p3.x;
		//_frame.vertices[3].pos[1] = p3.y;
		//_frame.vertices[3].pos[2] = 0.f;
		//_frame.vertices[3].pos[3] = w3;

		//_frame.vertices[3].uv[0] = 0.f;
		//_frame.vertices[3].uv[1] = 1.f;

		//_frame.vertices[3].color = color;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		if( m_framePack != NULL )
		{
			MovieKeyFrameManager::get()
				->releaseMovieFramePak( m_framePack );
		}

		ResourceReference::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::visitResourceMovie( ResourceMovieVisitor * _visitor )
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR("ResourceMovie::visitResourceMovie not compile"
				, m_name.c_str()
				);
		}

		for( TVectorMovieLayers2D::iterator
			it = m_layers2D.begin(),
			it_end = m_layers2D.end();
		it != it_end;
		++it )
		{
			const MovieLayer2D& layer = *it;
			const TVectorMovieFrameSource & frames = m_framePack->getLayerFrames( layer.index );
			_visitor->visitLayer2D( layer, frames );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}