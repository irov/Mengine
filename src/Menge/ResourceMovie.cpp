#	include "ResourceMovie.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/ConverterInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PrefetcherInterface.h"

//#	include "ResourceImageDefault.h"
#	include "ResourceShape.h"

#	include "Consts.h"

#	include "Core/Stream.h"

#	include "Logger/Logger.h"

#	include "Math/mat4.h"

#   include "Interface/MovieKeyFrameInterface.h"

#   include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_frameDuration(0.f)
		, m_duration(0.f)
        , m_loopSegment(0.f, 0.f)
		, m_size(0.f, 0.f)
		, m_maxLayerIndex(0)
		, m_hasCamera3D(false)
		, m_hasAnchorPoint(false)
		, m_anchorPoint(0.f, 0.f, 0.f)
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
    const mt::vec2f & ResourceMovie::getLoopSegment() const
    {
        return m_loopSegment;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceMovie::getFileName() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceMovie::getDataflowType() const
	{
		return m_dataflowType;
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
	ResourceShape * ResourceMovie::getSocketShape( const ConstString & _socketName ) const
	{
		for( TVectorMovieLayers::const_iterator 
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.name != _socketName )
			{
				continue;
			}
			
			ResourceShape * resourceShape = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceReferenceT<ResourceShape>( layer.source );
				
			return resourceShape;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::hasAnchorPoint() const
	{
		return m_hasAnchorPoint;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & ResourceMovie::getAnchorPoint() const
	{
		return m_anchorPoint;
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
		if( fabsf(m_frameDuration) < 0.0001f )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_isValid: '%s'm_frameDuration == 0.f"
				, this->getName().c_str()
				);

			return false;
		}

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

        if( m_path.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' don`t have Key Frames Pack Path"
                , this->getName().c_str()
                );

            return false;
        }

		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_path );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' invalid open file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

			return false;
		}

		MovieFramePackInterfacePtr framePack = DATA_SERVICE(m_serviceProvider)
			->dataflowT<MovieFramePackInterfacePtr>( m_dataflowType, stream );

		if( framePack == nullptr )
		{
			return false;
		}
				
        for( TVectorMovieLayers::const_iterator
            it = m_layers.begin(),
            it_end = m_layers.end();
        it != it_end;
        ++it )
        {
            const MovieLayer & layer = *it;

            if( framePack->hasLayer( layer.index ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' invalid layer %d '%s'"
                    , this->getName().c_str()
                    , layer.index
                    , layer.name.c_str()
                    );

                return false;
            }

			if( layer.layerType == CONST_STRING(m_serviceProvider, MovieSceneEffect) 
				|| layer.layerType == CONST_STRING(m_serviceProvider, MovieText)
				|| layer.layerType == CONST_STRING(m_serviceProvider, MovieTextCenter) 
				)
			{
				bool hide = framePack->isLayerPermanentlyHide( layer.index );

				if( hide == true )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' invalid layer '%d':'%s' permanently hide"
						, this->getName().c_str()
						, layer.index
						, layer.name.c_str()
						);

					return false;
				}
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
        metadata->get_Loop_Segment( m_loopSegment );

		m_hasAnchorPoint = metadata->get_Anchor_Point( m_anchorPoint );
                
        metadata->swap_KeyFramesPackPath_Path( m_path );
		metadata->swap_KeyFramesPackPath_Codec( m_dataflowType );
		metadata->swap_KeyFramesPackPath_Converter( m_converter );

		//FIX THIS
		if( m_dataflowType.empty() == true )
		{
			m_converter = CONST_STRING(m_serviceProvider, xmlToAekMovie);
		}

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

            ml.state = 0;

            meta_layer2d.swap_Name( ml.name );
            meta_layer2d.swap_Source( ml.source );
            meta_layer2d.swap_BlendingMode( ml.blendingMode );
			meta_layer2d.swap_Type( ml.layerType );

            ml.parent = meta_layer2d.get_Parent();
            ml.index = meta_layer2d.get_Index();
            ml.in = meta_layer2d.get_In();
            ml.out = meta_layer2d.get_Out();
            ml.startInterval = meta_layer2d.get_StartInterval();

            meta_layer2d.get_TimeRemap( ml.timeRemap );
            meta_layer2d.get_Shape( ml.shape );
            meta_layer2d.get_PlayCount( ml.playCount );
            meta_layer2d.get_Stretch( ml.scretch );
			meta_layer2d.get_Switch( ml.switcher );

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

            ml.state |= MOVIE_LAYER_THREED;

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

            meta_layer3d.get_TimeRemap( ml.timeRemap );
            meta_layer3d.get_Shape( ml.shape );
            meta_layer3d.get_PlayCount( ml.playCount );
            meta_layer3d.get_Stretch( ml.scretch );
			meta_layer3d.get_Switch( ml.switcher );
				
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

        for( TVectorMovieLayers::iterator
            it = m_layers.begin(),
            it_end = m_layers.end();
        it != it_end;
        ++it )
        {
            m_maxLayerIndex = std::max( m_maxLayerIndex, it->index );
            
			if( it->layerType == CONST_STRING(m_serviceProvider, MovieSlot) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieSceneEffect) )
            {
                it->state |= MOVIE_LAYER_NODE;

                it->parent = (size_t)-1;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieText) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
			else if( it->layerType == CONST_STRING(m_serviceProvider, MovieTextCenter) )
			{
				it->state |= MOVIE_LAYER_NODE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieNullObject) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, Image) )
            {				
                it->state |= MOVIE_LAYER_NODE;
				
				if( it->shape == true )
				{
					it->state |= MOVIE_LAYER_MESH_2D;
				}
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, SolidSprite) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieSocketImage) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieSocketShape) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, Animation) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, Video) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, Sound) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, ParticleEmitter) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
			else if( it->layerType == CONST_STRING(m_serviceProvider, ParticleEmitter2) )
			{
				it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
			}
            else if( it->layerType == CONST_STRING(m_serviceProvider, Movie) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, SubMovie) )
            {
                it->state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE | MOVIE_LAYER_SUB_MOVIE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieInternalObject) )
            {
                it->state |= MOVIE_LAYER_NODE;
            }
            else if( it->layerType == CONST_STRING(m_serviceProvider, MovieEvent) )
            {
                it->state |= MOVIE_LAYER_EXTRA;
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

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			if( this->isThreeDNode( it->index ) == true )
			{
				it->state |= MOVIE_LAYER_THREED;
			}
		}

        m_frameCount = (size_t)((m_duration / m_frameDuration) + 0.5f) - 1;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::isThreeDNode( size_t _index )
	{
		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			if( it->index != _index )
			{
				continue;
			}

			if( it->isThreeD() == true )
			{
				return true;
			}

			if( it->parent == 0 || it->parent == (size_t)-1 )
			{
				return false;
			}

			bool threeD = this->isThreeDNode( it->parent );

			return threeD;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_convert()
	{
		//bool result = this->convertDefault_( m_converter, m_path, m_path, m_codecType );

		if( m_path.empty() == true )
		{
			return false;
		}

		if( m_converter.empty() == false )
		{
			//FIX THIS

			PathString xml_path;
			
			xml_path += m_path;
			xml_path.replace_last( "xml" );

			ConstString c_xml_path = Helper::stringizeString( m_serviceProvider, xml_path );

			if( CONVERTER_SERVICE(m_serviceProvider)
				->convert( m_converter, m_category, c_xml_path, m_path ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_convert: '%s' can't convert '%s':'%s'"
					, this->getName().c_str() 
					, c_xml_path.c_str()
					, m_converter.c_str()
					);

				return false;
			}
		}

		if( m_dataflowType.empty() == true )
		{
			m_dataflowType = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_path );
		}

		if( m_dataflowType.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_convert: '%s' you must determine codec for file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

			return false;
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

		if( m_path.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				);

			return false;
		}

		const ConstString& category = this->getCategory();

		DataInterfacePtr data;
		if( PREFETCHER_SERVICE(m_serviceProvider)
			->getData( m_path, data ) == false )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( category, m_path );

			if( stream == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' don`t open Frames Pack '%s'"
					, this->getName().c_str()
					, m_path.c_str()
					);

				return false;
			}

			data = DATA_SERVICE(m_serviceProvider)
				->dataflow( m_dataflowType, stream );

			if( data == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' can` t get frame pack '%s'"
					, this->getName().c_str()
					, m_path.c_str()
					);

				return false;
			}
		}

		m_keyFramePack = data;

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			MovieLayer & layer = *it;

			const MovieLayerFrame & frame = m_keyFramePack->getLayer( layer.index );
			layer.immutable = frame.immutable;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		m_keyFramePack = nullptr;

		ResourceReference::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::visitResourceMovie( VisitorResourceMovie * _visitor )
	{
		if( this->isCompile() == false )
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
			
			_visitor->visitLayer( m_keyFramePack, layer );
		}
	}
}