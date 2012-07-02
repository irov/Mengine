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
		, m_duration(0.f)
		, m_width(0.f)
		, m_height(0.f)
		, m_framePack(NULL)
		, m_maxLayerIndex(0)
		, m_hasCamera3D(false)
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
	float ResourceMovie::getDuration() const
	{
		return m_duration;
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
	const TVectorMovieLayers & ResourceMovie::getLayers() const
	{
		return m_layers;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::hasCamera3D() const
	{
		return m_hasCamera3D;
	}
	//////////////////////////////////////////////////////////////////////////
	const MovieLayerCamera3D & ResourceMovie::getCamera3D() const
	{
		return m_camera3D;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp( float & _out, float _in1, float _in2, float _scale )
		{
			_out = _in1 + ( _in2 - _in1 ) * _scale; 
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp_v2( mt::vec2f & _out, const mt::vec2f & _in1, const mt::vec2f & _in2, float _scale )
		{
			s_linerp(_out.x, _in1.x, _in2.x, _scale);
			s_linerp(_out.y, _in1.y, _in2.y, _scale);
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp_f4( float * _out, const float * _frame1, const float * _frame2, float _scale )
		{
			s_linerp(_out[0], _frame1[0], _frame2[0], _scale);
			s_linerp(_out[1], _frame1[1], _frame2[1], _scale);
			s_linerp(_out[2], _frame1[2], _frame2[2], _scale);
			s_linerp(_out[3], _frame1[3], _frame2[3], _scale);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrame( const MovieLayer & _layer, size_t _index, MovieFrameSource & _frame ) const
	{
		if( m_framePack->getLayerFrame( _layer.index, _index, _frame ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameFirst( const MovieLayer & _layer, MovieFrameSource & _frame ) const
	{
		if( this->getFrame( _layer, 0, _frame ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::getFrameLast( const MovieLayer & _layer, MovieFrameSource & _frame ) const
	{
		if( m_framePack->isLayerEmpty( _layer.index ) == true )
		{
			return false;
		}

		size_t indexLast = m_framePack->getLayerCountFrames(_layer.index ) - 1;

		bool result = this->getFrame( _layer , indexLast, _frame );

		return result;
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
			BIN_CASE_ATTRIBUTE( Protocol::Duration_Value, m_duration );

			BIN_CASE_ATTRIBUTE( Protocol::Width_Value, m_width );
			BIN_CASE_ATTRIBUTE( Protocol::Height_Value, m_height );
			BIN_CASE_ATTRIBUTE( Protocol::KeyFramesPackPath_Path, m_keyFramePackPath );

			BIN_CASE_NODE( Protocol::MovieLayer2D )
			{
				m_layers.push_back( MovieLayer() );
				MovieLayer & ml = m_layers.back();

				ml.threeD = false;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Name, ml.name );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Parent, ml.parent );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Source, ml.source);
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_Out, ml.out );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_StartInterval, ml.startInterval );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer2D_BlendingMode, ml.blendingMode );
				}

				if( ml.in < 0.0f )
				{
					ml.in = 0.0f;
				}

				if( ml.out > m_duration )
				{
					ml.out = m_duration;
				}
				//ml.out -= m_frameDuration;

				//BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer2D_, ml );
			}

			BIN_CASE_NODE( Protocol::MovieLayer3D )
			{
				m_layers.push_back( MovieLayer() );
				MovieLayer & ml = m_layers.back();

				ml.threeD = true;

				MovieLayerCamera3D camera;

				bool depricated_internal;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Name, ml.name );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Parent, ml.parent );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Source, ml.source);
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Internal, depricated_internal ); //depricated
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_Out, ml.out );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_StartInterval, ml.startInterval );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer3D_BlendingMode, ml.blendingMode );
				}
				
				if( ml.in < 0.0f )
				{
					ml.in = 0.0f;
				}

				if( ml.out > m_duration )
				{
					ml.out = m_duration;
				}

				//mt::make_lookat_m4( camera.view, cameraPosition, cameraInterest );
				//mt::make_projection_m4( camera.projection, cameraFOV, cameraAspect, 1.f, 10000.f );

				//mt::mul_m4_m4( camera.vp, camera.view, camera.projection );

				//BIN_PARSE_METHOD_ARG2( this, &ResourceMovie::loaderMovieLayer3D_, ml, layer );
			}

			BIN_CASE_NODE( Protocol::MovieCamera3D )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_CameraPosition, m_camera3D.cameraPosition );
					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_CameraInterest, m_camera3D.cameraInterest );
					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_CameraFOV, m_camera3D.cameraFOV );
					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_CameraAspect, m_camera3D.cameraAspect );

					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_Width, m_camera3D.width );
					BIN_CASE_ATTRIBUTE( Protocol::MovieCamera3D_Height, m_camera3D.height );
				}

				m_hasCamera3D = true;

				//mt::make_lookat_m4( camera.view, cameraPosition, cameraInterest );
				//mt::make_projection_m4( camera.projection, cameraFOV, cameraAspect, 1.f, 10000.f );

				//mt::mul_m4_m4( camera.vp, camera.view, camera.projection );

				//m_layerCameras.insert( std::make_pair(ml.index, camera) );
				//BIN_PARSE_METHOD_ARG2( this, &ResourceMovie::loaderMovieLayer3D_, ml, layer );
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

		if( m_frameDuration == 0.f )
		{
			return false;
		}

		if( m_keyFramePackPath.empty() == true )
		{
			MENGE_LOG_ERROR("ResourceMovie::_compile: '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				);

			return false;
		}

		const ConstString& category = this->getCategory();

		m_framePack = MovieKeyFrameManager::get()
			->getMovieFramePak( category, m_keyFramePackPath );

		if ( m_framePack == NULL )
		{
			MENGE_LOG_ERROR("ResourceMovie::_compile: '%s' can` t get frame pack '%S'"
				, this->getName().c_str()
				, m_keyFramePackPath.c_str()
				);

			return false;
		}

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			m_maxLayerIndex = std::max( m_maxLayerIndex, it->index );

			if( it->source ==  Consts::get()->c_MovieSlot )
			{
				it->animatable = false;
				it->movie = false;

				continue;
			}
			
			if( it->source ==  Consts::get()->c_MovieNullObject )
			{
				it->animatable = false;
				it->movie = false;

				continue;
			}

			const ConstString & resourceType = ResourceManager::get()
				->getResourceType( it->source );

			if( resourceType == Consts::get()->c_ResourceImageDefault )
			{
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageCombineRGBAndAlpha )
			{
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageSolid )
			{
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageInAtlasCombineRGBAndAlpha )
			{
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceImageInAtlasDefault )
			{
				it->animatable = false;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceAnimation )
			{
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceVideo )
			{
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceSound )
			{
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceEmitterContainer )
			{
				it->animatable = true;
				it->movie = false;
			}
			else if( resourceType == Consts::get()->c_ResourceMovie )
			{
				it->animatable = true;
				it->movie = true;
			}
			else if( resourceType == Consts::get()->c_ResourceInternalObject )
			{
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
		
		m_frameCount = (size_t)((m_duration / m_frameDuration) + 0.5f) - 1;

		return true;
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

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			const TVectorMovieFrameSource & frames = m_framePack->getLayerFrames( layer.index );

			_visitor->visitLayer( layer, frames );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}