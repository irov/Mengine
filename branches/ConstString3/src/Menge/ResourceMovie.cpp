#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "ResourceImageDefault.h"

#	include "Metacode.h"
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
	void ResourceMovie::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceMovie * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceMovie *>(_meta);

        metadata->get_Duration_Value( m_duration );
        metadata->get_FrameDuration_Value( m_frameDuration );
        metadata->get_Width_Value( m_width );
        metadata->get_Height_Value( m_height );
        
        metadata->swap_KeyFramesPackPath_Path( m_keyFramePackPath );
		
        const Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieLayer2D & includes_layer2d = metadata->get_IncludesMovieLayer2D();

        for( Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieLayer2D::const_iterator
            it = includes_layer2d.begin(),
            it_end = includes_layer2d.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & meta_layer2d = *it;

            m_layers.push_back( MovieLayer() );
            MovieLayer & ml = m_layers.back();

            ml.threeD = false;

            meta_layer2d.swap_Name( ml.name );
            meta_layer2d.swap_Source( ml.source );
            meta_layer2d.swap_BlendingMode( ml.blendingMode );

            ml.parent = meta_layer2d.get_Parent();
            ml.index = meta_layer2d.get_Index();
            ml.in = meta_layer2d.get_In();
            ml.out = meta_layer2d.get_Out();
            ml.startInterval = meta_layer2d.get_StartInterval();
            
            meta_layer2d.get_PlayCount( ml.playCount );

            if( ml.in < 0.f )
            {
                ml.in = 0.f;
            }

            if( ml.out > m_duration )
            {
                ml.out = m_duration;
            }
        }

        const Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieLayer3D & includes_layer3d = metadata->get_IncludesMovieLayer3D();

        for( Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieLayer3D::const_iterator
            it = includes_layer3d.begin(),
            it_end = includes_layer3d.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & meta_layer3d = *it;

            m_layers.push_back( MovieLayer() );
            MovieLayer & ml = m_layers.back();

            ml.threeD = true;

            MovieLayerCamera3D camera;

            meta_layer3d.swap_Name( ml.name );
            meta_layer3d.swap_Source( ml.source );
            meta_layer3d.swap_BlendingMode( ml.blendingMode );

            ml.parent = meta_layer3d.get_Parent();
            ml.index = meta_layer3d.get_Index();
            ml.in = meta_layer3d.get_In();
            ml.out = meta_layer3d.get_Out();
            ml.startInterval = meta_layer3d.get_StartInterval();
				
            if( ml.in < 0.f )
            {
                ml.in = 0.f;
            }

            if( ml.out > m_duration )
            {
                ml.out = m_duration;
            }
        }


        const Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieCamera3D & includes_camera3d = metadata->get_IncludesMovieCamera3D();

        for( Metacode::Meta_DataBlock::Meta_ResourceMovie::TVectorMeta_MovieCamera3D::const_iterator
            it = includes_camera3d.begin(),
            it_end = includes_camera3d.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & meta_camera3d = *it;

            m_camera3D.cameraPosition = meta_camera3d.get_CameraPosition();
            m_camera3D.cameraInterest = meta_camera3d.get_CameraInterest();
            m_camera3D.cameraFOV = meta_camera3d.get_CameraFOV();
            m_camera3D.cameraAspect = meta_camera3d.get_CameraAspect();
            m_camera3D.width = meta_camera3d.get_Width();
            m_camera3D.height = meta_camera3d.get_Height();

            m_hasCamera3D = true;
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
	void ResourceMovie::visitResourceMovie( VisitorResourceMovie * _visitor )
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

			const MovieLayerFrame & frames = m_framePack->getLayer( layer.index );

			_visitor->visitLayer( layer, frames );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}