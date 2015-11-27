#	include "ResourceMovie.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/ConverterInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/TextInterface.h"

//#	include "ResourceImageDefault.h"
#	include "ResourceShape.h"

#	include "Consts.h"

#	include "Core/Stream.h"

#	include "Logger/Logger.h"

#	include "Math/mat4.h"

#   include "Interface/MovieKeyFrameInterface.h"

#   include <algorithm>
#   include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_frameDuration( 0.f )
		, m_duration( 0.f )
		, m_loopSegment( 0.f, -1.f )
		, m_size( 0.f, 0.f )
		, m_maxLayerIndex( 0 )
		, m_hasCamera3D( false )
		, m_hasBoundBox( false )
		, m_hasAnchorPoint( false )
		, m_anchorPoint( 0.f, 0.f, 0.f )
		, m_hasOffsetPoint( false )
		, m_offsetPoint( 0.f, 0.f, 0.f )
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
	uint32_t ResourceMovie::getFrameCount() const
	{
		return m_frameCount;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceMovie::getMaxLayerIndex() const
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
	const FilePath & ResourceMovie::getFilePath() const
	{
		return m_filePath;
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
	ResourceShape * ResourceMovie::getSocketResourceShape( const ConstString & _socketName ) const
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
				->getResourceReferenceT<ResourceShape *>( layer.source );
				
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
	bool ResourceMovie::hasOffsetPoint() const
	{
		return m_hasOffsetPoint;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & ResourceMovie::getOffsetPoint() const
	{
		return m_offsetPoint;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::hasBoundBox() const
	{
		return m_hasBoundBox;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f & ResourceMovie::getBoundBox() const
	{
		return m_boundbox;
	}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class FindResourceMovieValidParent
        {
        public:
            FindResourceMovieValidParent( uint32_t _parent )
                : m_parent(_parent)
            {
            }

        public:
            bool operator() ( const MovieLayer & _layer ) const
            {
                if( _layer.index != m_parent )
                {
                    return false;
                }

                return true;
            }

        protected:
            uint32_t m_parent;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::_isValid() const
    {
		if( mt::equal_f_z( m_frameDuration ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_isValid: '%s' group '%s' m_frameDuration == 0.f"
				, this->getName().c_str()
				, this->getGroup().c_str()
				);

			return false;
		}

		uint32_t limitMovieWidth = CONFIG_VALUE( m_serviceProvider, "Limit", "MovieWidth", 16384U );
		uint32_t limitMovieHeight = CONFIG_VALUE( m_serviceProvider, "Limit", "MovieHeight", 16384U );

		uint32_t width = (uint32_t)(m_size.x + 0.5f);
		uint32_t height = (uint32_t)(m_size.y + 0.5f);

		if( (width > limitMovieWidth && limitMovieWidth != 0U) || (height > limitMovieHeight && limitMovieHeight != 0U) )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid '%s' group '%s' invalid limit %d:%d size %d:%d"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, limitMovieWidth
				, limitMovieHeight
				, width
				, height
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

			if( layer.parent == 0 )
			{
				continue;
			}

			if( layer.parent == movie_layer_parent_none )
			{
				continue;
			}
            
            FindResourceMovieValidParent frmvp(layer.parent);
            if( std::find_if( m_layers.begin(), m_layers.end(), frmvp ) == m_layers.end() )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid '%s' group '%s' layer %s:%d invalid parent %d"
                    , this->getName().c_str()
					, this->getGroup().c_str()
                    , layer.name.c_str()
                    , layer.index
                    , layer.parent
                    );

                return false;
            }
        }

        if( m_filePath.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' don`t have Key Frames Pack Path"
                , this->getName().c_str()
				, this->getGroup().c_str()
                );

            return false;
        }

		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_filePath, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' invalid open file '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		ConstString dataflowType = m_dataflowType;

		if( dataflowType.empty() == true )
		{
			dataflowType = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_filePath );

			if( dataflowType.empty() == true )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' you must determine codec for file '%s'"
					, this->getName().c_str()
					, this->getGroup().c_str()
					, m_filePath.c_str()
					);

				return false;
			}
		}

		MovieFramePackInterfacePtr framePack = DATA_SERVICE(m_serviceProvider)
			->dataflowT<MovieFramePackInterfacePtr>( dataflowType, stream );

		if( framePack == nullptr )
		{
			return false;
		}
				
		bool layers_successful = true;

        for( TVectorMovieLayers::const_iterator
            it = m_layers.begin(),
            it_end = m_layers.end();
        it != it_end;
        ++it )
        {
            const MovieLayer & layer = *it;

            if( framePack->hasLayer( layer.index ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' invalid layer %d '%s' type '%s'"
                    , this->getName().c_str()
					, this->getGroup().c_str()
                    , layer.index
                    , layer.name.c_str()
					, layer.type.c_str()
                    );

				layers_successful = false;

                continue;
            }

			if( layer.type == CONST_STRING( m_serviceProvider, MovieSceneEffect ) )
			{
				if( layer.isThreeD() == true )
				{
					LOGGER_ERROR( m_serviceProvider )("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d' SceneEffect should not be threeD"
						, this->getName().c_str()
						, this->getGroup().c_str()
						, layer.index
						);

					layers_successful = false;

					continue;
				}
			}

			float MovieImageScale = CONFIG_VALUE( m_serviceProvider, "Limit", "MovieImageScale", 0.75f );

			if( layer.type == CONST_STRING(m_serviceProvider, Image)
				|| layer.type == CONST_STRING( m_serviceProvider, Animation )
				|| layer.type == CONST_STRING( m_serviceProvider, Video )
				)
			{
				const MovieLayerFrame & layerFrame = framePack->getLayer( layer.index );
				
				if( layerFrame.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_SCALE || layerFrame.immutable == true )
				{
					if( ::fabsf( layerFrame.source.scale.x ) < MovieImageScale || ::fabsf( layerFrame.source.scale.y ) < MovieImageScale )
					{
						LOGGER_ERROR( m_serviceProvider )("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d':'%s' type '%s' immutable and scale %f:%f (please rescale on graphics editor and re-export)"
							, this->getName().c_str()
							, this->getGroup().c_str()
							, layer.index
							, layer.name.c_str()
							, layer.type.c_str()
							, layerFrame.source.scale.x
							, layerFrame.source.scale.y
							);

						layers_successful = false;

						continue;
					}
				}
				else
				{
					float scale_max_x = 0.f;
					float scale_max_y = 0.f;

					for( uint32_t i = 0; i != layerFrame.count; ++i )
					{
						if( ::fabsf( layerFrame.scale[i].x ) > scale_max_x )
						{
							scale_max_x = ::fabsf( layerFrame.scale[i].x );
						}

						if( ::fabsf( layerFrame.scale[i].y ) > scale_max_y )
						{
							scale_max_y = ::fabsf( layerFrame.scale[i].y );
						}
					}

					if( scale_max_x < MovieImageScale || scale_max_y < MovieImageScale )
					{
						LOGGER_ERROR( m_serviceProvider )("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d':'%s' type '%s' immutable and scale %f:%f (please rescale on graphics editor and re-export)"
							, this->getName().c_str()
							, this->getGroup().c_str()
							, layer.index
							, layer.name.c_str()
							, layer.type.c_str()
							, scale_max_x
							, scale_max_y
							);

						layers_successful = false;

						continue;
					}
				}
			}

			if( layer.type == CONST_STRING(m_serviceProvider, MovieSceneEffect) 
				|| layer.type == CONST_STRING(m_serviceProvider, MovieText)
				|| layer.type == CONST_STRING(m_serviceProvider, MovieTextCenter) 				
				|| layer.type == CONST_STRING(m_serviceProvider, Sound)
				|| layer.type == CONST_STRING(m_serviceProvider, SoundId)
				|| layer.type == CONST_STRING(m_serviceProvider, MovieSprite)
				|| layer.type == CONST_STRING(m_serviceProvider, MovieSlot)
				)
			{
				bool hide = framePack->isLayerPermanentlyHide( layer.index );

				if( hide == true )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d':'%s' type '%s' permanently hide"
						, this->getName().c_str()
						, this->getGroup().c_str()
						, layer.index
						, layer.name.c_str()
						, layer.type.c_str()
						);

					layers_successful = false;

					continue;
				}
			}

			if( layer.type == CONST_STRING(m_serviceProvider, MovieText)
				|| layer.type == CONST_STRING(m_serviceProvider, MovieTextCenter)
				)
			{
				const TextEntryInterface * entry;
				if( TEXT_SERVICE(m_serviceProvider)
					->existText( layer.name, &entry ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d':'%s' type '%s' text %s not found"
						, this->getName().c_str()
						, this->getGroup().c_str()
						, layer.index
						, layer.name.c_str()
						, layer.type.c_str()
						, layer.name.c_str()
						);

					layers_successful = false;

					continue;
				}
			}

			if( layer.type == CONST_STRING( m_serviceProvider, ParticleEmitter2 )
				|| layer.type == CONST_STRING( m_serviceProvider, Video ) )
			{
				if( mt::equal_f_z( layer.startInterval ) == false )
				{
					LOGGER_ERROR( m_serviceProvider )("ResourceMovie::isValid: '%s' group '%s' invalid layer '%d':'%s' type '%s' start interval is not support"
						, this->getName().c_str()
						, this->getGroup().c_str()
						, layer.index
						, layer.name.c_str()
						, layer.type.c_str()
						, layer.name.c_str()
						);

					layers_successful = false;
				}
			}
        }

        return layers_successful;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceMovie * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceMovie *>(_meta);

        m_duration = metadata->get_Duration_Value();
        m_frameDuration = metadata->get_FrameDuration_Value();
        m_size.x = metadata->get_Width_Value();
        m_size.y = metadata->get_Height_Value();
        
		metadata->get_Loop_Segment( m_loopSegment );

		m_hasBoundBox = metadata->get_Bounds_Box( m_boundbox );
		m_hasAnchorPoint = metadata->get_Anchor_Point( m_anchorPoint );
		m_hasOffsetPoint = metadata->get_Offset_Point( m_offsetPoint );
                
        metadata->swap_KeyFramesPackPath_Path( m_filePath );
		metadata->swap_KeyFramesPackPath_Codec( m_dataflowType );
		metadata->swap_KeyFramesPackPath_Converter( m_converterType );

		//FIX THIS
		if( m_dataflowType.empty() == true )
		{
			m_converterType = CONST_STRING(m_serviceProvider, xmlToAekMovie);
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
            meta_layer2d.swap_Type( ml.type );
			            
            ml.index = meta_layer2d.get_Index();
            ml.in = meta_layer2d.get_In();
            ml.out = meta_layer2d.get_Out();

			meta_layer2d.get_Parent( ml.parent );
            meta_layer2d.get_StartInterval( ml.startInterval );
			meta_layer2d.swap_BlendingMode( ml.blendingMode );

            meta_layer2d.get_TimeRemap( ml.timeRemap );
            meta_layer2d.get_Shape( ml.shape );
			meta_layer2d.get_Polygon( ml.polygon );
            meta_layer2d.get_PlayCount( ml.playCount );
            meta_layer2d.get_Stretch( ml.stretch );
			meta_layer2d.get_Switch( ml.switcher );
			meta_layer2d.get_Loop( ml.loop );
			meta_layer2d.get_Position( ml.position );
			ml.hasViewport = meta_layer2d.get_Viewport( ml.viewport );
			meta_layer2d.get_Params( ml.params );

			if( ml.loop == false )
			{
				if( ml.in < 0.f )
				{
					ml.startInterval -= ml.in;
					ml.in = 0.f;
				}

				if( ml.out > m_duration )
				{
					ml.out = m_duration;
				}
			}
			else
			{
				ml.startInterval = 0.f;

				ml.in = 0.f;
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
			meta_layer3d.swap_Type( ml.type );

            
            ml.index = meta_layer3d.get_Index();
            ml.in = meta_layer3d.get_In();
            ml.out = meta_layer3d.get_Out();

			meta_layer3d.get_Parent( ml.parent );
            meta_layer3d.get_StartInterval( ml.startInterval );
			meta_layer3d.swap_BlendingMode( ml.blendingMode );

            meta_layer3d.get_TimeRemap( ml.timeRemap );
            meta_layer3d.get_Shape( ml.shape );
			meta_layer3d.get_Polygon( ml.polygon );
            meta_layer3d.get_PlayCount( ml.playCount );
            meta_layer3d.get_Stretch( ml.stretch );
			meta_layer3d.get_Switch( ml.switcher );
			meta_layer3d.get_Loop( ml.loop );
			meta_layer3d.get_Position( ml.position );
			meta_layer3d.get_Params( ml.params );
				
			if( ml.loop == false )
			{
				if( ml.in < 0.f )
				{
					ml.startInterval -= ml.in;
					ml.in = 0.f;
				}

				if( ml.out > m_duration )
				{
					ml.out = m_duration;
				}
			}
			else
			{
				ml.startInterval = 0.f;

				ml.in = 0.f;
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
			MovieLayer & layer = *it;

			m_maxLayerIndex = m_maxLayerIndex > layer.index ? m_maxLayerIndex : layer.index;
            
			if( layer.type == CONST_STRING( m_serviceProvider, MovieSlot ) )
            {
				layer.state |= MOVIE_LAYER_NODE;
            }
			else if( layer.type == CONST_STRING( m_serviceProvider, MovieSceneEffect ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_SCENE_EFFECT;

                layer.parent = movie_layer_parent_none;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieText) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieTextCenter) )
			{
				layer.state |= MOVIE_LAYER_NODE;
			}
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieNullObject) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, Image) )
            {				
                layer.state |= MOVIE_LAYER_NODE;
				
				if( layer.shape == true )
				{
					layer.state |= MOVIE_LAYER_MESH_2D;
				}
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, SolidSprite) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieSocketImage) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieSocketShape) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, Animation) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, Video) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, Sound) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO;
            }
			else if( layer.type == CONST_STRING(m_serviceProvider, SoundId) )
			{
				layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO | MOVIE_LAYER_UNSTOPPABLE;
			}
			else if( layer.type == CONST_STRING(m_serviceProvider, MovieSprite) )
			{
				layer.state |= MOVIE_LAYER_NODE;
			}
            else if( layer.type == CONST_STRING(m_serviceProvider, ParticleEmitter) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
			else if( layer.type == CONST_STRING(m_serviceProvider, ParticleEmitter2) )
			{
				layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
			}
            else if( layer.type == CONST_STRING(m_serviceProvider, Movie) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, SubMovie) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE | MOVIE_LAYER_SUB_MOVIE;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieInternalObject) )
            {
				layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_INTERNAL;
            }
            else if( layer.type == CONST_STRING(m_serviceProvider, MovieEvent) )
            {
                layer.state |= MOVIE_LAYER_EXTRA;
            }
            else
            {
				LOGGER_ERROR( m_serviceProvider )("ResourceMovie: '%s' group '%s' can't setup layer2d '%s' type '%s'"
                    , this->getName().c_str()
					, this->getGroup().c_str()
                    , layer.source.c_str()
                    , layer.type.c_str()
                    );

                return false;
            }

            if( mt::equal_f_z( layer.in ) == true && 
				mt::equal_f_f_e( layer.out, m_duration, m_frameDuration ) == true &&
				mt::equal_f_z( layer.startInterval ) == true )
            {
                layer.loop = true;
            }
        }

		for( TVectorMovieLayers::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			MovieLayer & layer = *it;

			if( this->isThreeDNode( layer.index ) == true )
			{
				layer.state |= MOVIE_LAYER_THREED;
			}
		}

        m_frameCount = (uint32_t)((m_duration / m_frameDuration) + 0.5f) - 1;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::isThreeDNode( uint32_t _index ) const
	{
		for( TVectorMovieLayers::const_iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.index != _index )
			{
				continue;
			}

			if( layer.isThreeD() == true )
			{
				return true;
			}

			if( layer.parent == 0 || layer.parent == movie_layer_parent_none )
			{
				return false;
			}

			bool threeD = this->isThreeDNode( layer.parent );

			return threeD;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_convert()
	{
		//bool result = this->convertDefault_( m_converter, m_path, m_path, m_codecType );

		if( m_filePath.empty() == true )
		{
			return false;
		}

		if( m_converterType.empty() == false )
		{
			//FIX THIS

			PathString xml_path;
			
			xml_path += m_filePath;
			xml_path.replace_last( "xml" );

			ConstString c_xml_path = Helper::stringizeString( m_serviceProvider, xml_path );

			if( CONVERTER_SERVICE(m_serviceProvider)
				->convert( m_converterType, m_category, c_xml_path, m_filePath ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_convert: '%s' group '%s' can't convert '%s':'%s'"
					, this->getName().c_str() 
					, this->getGroup().c_str()
					, c_xml_path.c_str()
					, m_converterType.c_str()
					);

				return false;
			}
		}

		if( m_dataflowType.empty() == true )
		{
			m_dataflowType = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_filePath );

			if( m_dataflowType.empty() == true )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_convert: '%s' group '%s' you must determine codec for file '%s'"
					, this->getName().c_str()
					, this->getGroup().c_str()
					, m_filePath.c_str()
					);

				return false;
			}
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

		if( m_filePath.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' group '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				, this->getGroup().c_str()
				);

			return false;
		}

		const ConstString & category = this->getCategory();

		DataInterfacePtr data = this->compileData_( category, m_filePath );

		if( data == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' group '%s' can` t get frame pack '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		m_keyFramePack = data;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DataInterfacePtr ResourceMovie::compileData_( const ConstString & _category, const FilePath & _path )
	{
		DataInterfacePtr prefetch_data;
		if( PREFETCHER_SERVICE(m_serviceProvider)
			->getData( _category, _path, prefetch_data ) == true )
		{
			return prefetch_data;
		}

		const ConstString & category = this->getCategory();
				
		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, _path, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::compileData_: '%s' group '%s' don`t open Frames Pack '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		DataInterfacePtr data = DATA_SERVICE(m_serviceProvider)
			->dataflow( m_dataflowType, stream );

		if( data == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::compileData_: '%s' group '%s' can` t dataflow '%s' from '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_dataflowType.c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		return data;
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
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::visitResourceMovie '%s' group '%s' not compile"
				, this->getName().c_str()
				, this->getGroup().c_str()
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
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::hasMovieLayer( const ConstString & _name, const MovieLayer ** _layer ) const
	{
		for( TVectorMovieLayers::const_iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.name != _name )
			{
				continue;
			}
			
			*_layer = &layer;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::hasMovieLayerType( const ConstString & _name, const ConstString & _type, const MovieLayer ** _layer ) const
	{
		for( TVectorMovieLayers::const_iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			const MovieLayer & layer = *it;

			if( layer.name != _name )
			{
				continue;
			}

			if( layer.type != _type )
			{
				continue;
			}

			*_layer = &layer;

			return true;
		}

		return false;
	}

}