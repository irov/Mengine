#	include "ResourceMovie.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"

#	include "ResourceImageDefault.h"

#	include "Metacode.h"
#	include "Consts.h"

#	include "Core/Stream.h"

#	include "Logger/Logger.h"

#	include "Math/mat4.h"

#   include "Interface/MovieKeyFrameInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_frameDuration(0.f)
		, m_duration(0.f)
		, m_size(0.f, 0.f)
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
	const mt::vec2f & ResourceMovie::getSize() const
	{
		return m_size;
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
	//namespace Helper
	//{
		////////////////////////////////////////////////////////////////////////////
		//static void s_linerp( float & _out, float _in1, float _in2, float _scale )
		//{
		//	_out = _in1 + ( _in2 - _in1 ) * _scale; 
		//}
		////////////////////////////////////////////////////////////////////////////
		//static void s_linerp_v2( mt::vec2f & _out, const mt::vec2f & _in1, const mt::vec2f & _in2, float _scale )
		//{
		//	s_linerp(_out.x, _in1.x, _in2.x, _scale);
		//	s_linerp(_out.y, _in1.y, _in2.y, _scale);
		//}
		////////////////////////////////////////////////////////////////////////////
		//static void s_linerp_f4( float * _out, const float * _frame1, const float * _frame2, float _scale )
		//{
		//	s_linerp(_out[0], _frame1[0], _frame2[0], _scale);
		//	s_linerp(_out[1], _frame1[1], _frame2[1], _scale);
		//	s_linerp(_out[2], _frame1[2], _frame2[2], _scale);
		//	s_linerp(_out[3], _frame1[3], _frame2[3], _scale);
		//}
	//}
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
    namespace
    {
        class FindResourceMovieValidParent
        {
        public:
            FindResourceMovieValidParent( size_t _parent )
                : m_parent(_parent)
            {
            }

        public:
            bool operator() ( const MovieLayer & _layer ) const
            {
                if( _layer.parent != m_parent )
                {
                    return false;
                }

                return true;
            }

        protected:
            size_t m_parent;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::_isValid() const
    {
        for( TVectorMovieLayers::const_iterator
            it = m_layers.begin(),
            it_end = m_layers.end();
        it != it_end;
        ++it )
        {
            const MovieLayer & layer = *it;
            
            FindResourceMovieValidParent frmvp(layer.parent);
            if( std::find_if( m_layers.begin(), m_layers.end(), frmvp ) == m_layers.end() )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid %s layer %s:%d invalid parent %d"
                    , this->getName().c_str()
                    , layer.name.c_str()
                    , layer.index
                    , layer.parent
                    );

                return false;
            }
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceMovie * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceMovie *>(_meta);

        metadata->get_Duration_Value( m_duration );
        metadata->get_FrameDuration_Value( m_frameDuration );
        metadata->get_Width_Value( m_size.x );
        metadata->get_Height_Value( m_size.y );
        
        metadata->swap_KeyFramesPackPath_Path( m_keyFramePackPath );
		
        m_layers.clear();

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

            meta_layer2d.swap_Name( ml.name );
            meta_layer2d.swap_Source( ml.source );
            meta_layer2d.swap_BlendingMode( ml.blendingMode );
			meta_layer2d.swap_Type( ml.layerType );

            ml.parent = meta_layer2d.get_Parent();
            ml.index = meta_layer2d.get_Index();
            ml.in = meta_layer2d.get_In();
            ml.out = meta_layer2d.get_Out();
            ml.startInterval = meta_layer2d.get_StartInterval();
            
            meta_layer2d.get_PlayCount( ml.playCount );

            if( ml.in < 0.f )
            {
                ml.startInterval += -ml.in;
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

            ml.state = MOVIE_LAYER_THREED;

            MovieLayerCamera3D camera;

            meta_layer3d.swap_Name( ml.name );
            meta_layer3d.swap_Source( ml.source );
            meta_layer3d.swap_BlendingMode( ml.blendingMode );
			meta_layer3d.swap_Type( ml.layerType );

            ml.parent = meta_layer3d.get_Parent();
            ml.index = meta_layer3d.get_Index();
            ml.in = meta_layer3d.get_In();
            ml.out = meta_layer3d.get_Out();
            ml.startInterval = meta_layer3d.get_StartInterval();
				
            if( ml.in < 0.f )
            {
                ml.startInterval += -ml.in;
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

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		if( fabsf(m_frameDuration) < 0.0001f )
		{
            LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s'm_frameDuration == 0.f"
                , this->getName().c_str()
                );

			return false;
		}

		if( m_keyFramePackPath.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				);

			return false;
		}

		const ConstString& category = this->getCategory();

		m_framePack = MOVIEKEYFRAME_SERVICE(m_serviceProvider)
			->getMovieFramePak( category, m_keyFramePackPath );

		if ( m_framePack == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' can` t get frame pack '%s'"
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
															
			if( it->layerType == CONST_STRING(m_serviceProvider, MovieSlot) )
			{
                it->state = MOVIE_LAYER_NODE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieSceneEffect) )
            {
                it->state = MOVIE_LAYER_NODE;

                it->parent = (size_t)-1;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieText) )
            {
                it->state = MOVIE_LAYER_NODE;
            }
			else if( it->layerType == CONST_STRING(m_serviceProvider, MovieNullObject) )
			{
                it->state = MOVIE_LAYER_NODE;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, Image) )
			{
                it->state = MOVIE_LAYER_NODE;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, SolidSprite) )
			{
                it->state = MOVIE_LAYER_NODE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieSocket) )
            {
                it->state = MOVIE_LAYER_NODE;
            }
			else if( it->layerType == CONST_STRING(m_serviceProvider, Animation) )
			{
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, Video) )
			{
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, Sound) )
			{
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, ParticleEmitter) )
			{
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
			}
			else if( it->layerType == CONST_STRING(m_serviceProvider, Movie) )
			{
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, SubMovie) )
            {
                it->state = MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE | MOVIE_LAYER_SUB_MOVIE;
            }
			else if( it->layerType == CONST_STRING(m_serviceProvider, MovieInternalObject) )
			{
                it->state = MOVIE_LAYER_NODE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieEvent) )
            {
                it->state = MOVIE_LAYER_EXTRA;
            }
			else
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie: '%s' can't setup layer2d '%s' type '%s'"
					, this->getName().c_str()
					, it->source.c_str()
					, it->layerType.c_str()
					);

				return false;
            }

            if( fabsf( it->in - 0.f ) < 0.0001f && fabsf( it->out - m_duration ) < 0.0001f )
            {
                it->loop = true;
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
			MOVIEKEYFRAME_SERVICE(m_serviceProvider)
				->releaseMovieFramePak( m_framePack );
		}

		ResourceReference::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::visitResourceMovie( VisitorResourceMovie * _visitor )
	{
		if( isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::visitResourceMovie not compile"
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