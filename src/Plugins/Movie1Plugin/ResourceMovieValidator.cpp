#include "ResourceMovieValidator.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Movie1Interface.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/Algorithm.h"
#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMovieValidator::ResourceMovieValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovieValidator::~ResourceMovieValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovieValidator::_validate( const ResourceMoviePtr & _resource ) const
    {
        float frameDuration = _resource->getFrameDuration();

        if( mt::equal_f_z( frameDuration ) == true )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' m_frameDuration == 0.f"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        uint32_t Limit_MovieWidth = CONFIG_VALUE( "Limit", "MovieWidth", 16384U );
        uint32_t Limit_MovieHeight = CONFIG_VALUE( "Limit", "MovieHeight", 16384U );

        const mt::vec2f & size = _resource->getSize();

        uint32_t width = (uint32_t)(size.x + 0.5f);
        uint32_t height = (uint32_t)(size.y + 0.5f);

        if( (width > Limit_MovieWidth && Limit_MovieWidth != 0U) || (height > Limit_MovieHeight && Limit_MovieHeight != 0U) )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid limit %u:%u size %u:%u"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , Limit_MovieWidth
                , Limit_MovieHeight
                , width
                , height
            );

            return false;
        }

        const VectorMovieLayers & layers = _resource->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.parent == 0 )
            {
                continue;
            }

            if( layer.parent == movie_layer_parent_none )
            {
                continue;
            }

            uint32_t layer_parent = layer.parent;
            if( Algorithm::find_if( layers.begin(), layers.end(), [layer_parent]( const MovieLayer & _layer )
            {
                return _layer.index == layer_parent;
            } ) == layers.end() )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "release '%s' group '%s' layer '%s' index [%u] invalid parent [%u]"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , layer.name.c_str()
                    , layer.index
                    , layer.parent
                );

                return false;
            }
        }

        const ContentInterfacePtr & content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "release '%s' group '%s' don`t have Key Frames Pack Path"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "release '%s' group '%s' invalid open file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            return false;
        }

        DataflowInterfacePtr dataflow = content->getDataflow();

        if( dataflow == nullptr )
        {
            const ConstString & dataflowType = CODEC_SERVICE()
                ->findCodecType( filePath );

            if( dataflowType.empty() == true )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "release '%s' group '%s' you must determine codec for file '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , _resource->getContent()->getFilePath().c_str()
                );

                return false;
            }

            dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

            if( dataflow == nullptr )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "release '%s' group '%s' not found dataflow type '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , dataflowType.c_str()
                );

                return false;
            }
        }

        MovieFramePackInterfacePtr framePack = DATA_SERVICE()
            ->dataflow( dataflow, stream, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        if( framePack == nullptr )
        {
            return false;
        }

        bool layers_successful = true;

        for( const MovieLayer & layer : layers )
        {
            if( framePack->hasLayer( layer.index ) == false )
            {
                LOGGER_MESSAGE_RELEASE_ERROR( "'%s' group '%s' invalid layer [%u] name '%s' type '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , layer.index
                    , layer.name.c_str()
                    , layer.type.c_str()
                );

                layers_successful = false;

                continue;
            }

            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) )
            {
                if( layer.isThreeD() == true )
                {
                    LOGGER_MESSAGE_RELEASE_ERROR( "'%s' group '%s' invalid layer [%u] SceneEffect should not be threeD"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , layer.index
                    );

                    layers_successful = false;

                    continue;
                }
            }

            float Limit_MovieImageScale = CONFIG_VALUE( "Limit", "MovieImageScale", 0.75f );

            if( layer.type == STRINGIZE_STRING_LOCAL( "Image" )
                || layer.type == STRINGIZE_STRING_LOCAL( "Animation" )
                || layer.type == STRINGIZE_STRING_LOCAL( "Video" )
                )
            {
                const MovieLayerFrame & layerFrame = framePack->getLayer( layer.index );

                if( layerFrame.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_SCALE || layerFrame.immutable == true )
                {
                    if( MENGINE_FABSF( layerFrame.source.scale.x ) < Limit_MovieImageScale ||
                        MENGINE_FABSF( layerFrame.source.scale.y ) < Limit_MovieImageScale )
                    {
                        LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid layer [%u] name '%s' type '%s' immutable and scale %f:%f (please rescale on graphics editor and re-export)"
                            , _resource->getName().c_str()
                            , _resource->getGroupName().c_str()
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
                        if( MENGINE_FABSF( layerFrame.scale[i].x ) > scale_max_x )
                        {
                            scale_max_x = MENGINE_FABSF( layerFrame.scale[i].x );
                        }

                        if( MENGINE_FABSF( layerFrame.scale[i].y ) > scale_max_y )
                        {
                            scale_max_y = MENGINE_FABSF( layerFrame.scale[i].y );
                        }
                    }

                    if( scale_max_x < Limit_MovieImageScale || scale_max_y < Limit_MovieImageScale )
                    {
                        LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid layer [%u] name '%s' type '%s' minmax and scale %f:%f (please rescale on graphics editor and re-export)"
                            , _resource->getName().c_str()
                            , _resource->getGroupName().c_str()
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

            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSceneEffect" )
                || layer.type == STRINGIZE_STRING_LOCAL( "MovieText" )
                || layer.type == STRINGIZE_STRING_LOCAL( "MovieTextCenter" )
                || layer.type == STRINGIZE_STRING_LOCAL( "Sound" )
                || layer.type == STRINGIZE_STRING_LOCAL( "SoundId" )
                || layer.type == STRINGIZE_STRING_LOCAL( "MovieSprite" )
                || layer.type == STRINGIZE_STRING_LOCAL( "MovieSlot" )
                )
            {
                bool hide = framePack->isLayerPermanentlyHide( layer.index );

                if( hide == true )
                {
                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid layer [%u] name '%s' type '%s' permanently hide"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , layer.index
                        , layer.name.c_str()
                        , layer.type.c_str()
                    );

                    layers_successful = false;

                    continue;
                }
            }

            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieText" )
                || layer.type == STRINGIZE_STRING_LOCAL( "MovieTextCenter" )
                )
            {
                TextEntryInterfacePtr entry;
                if( TEXT_SERVICE()
                    ->hasTextEntry( layer.name, &entry ) == false )
                {
                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' invalid layer [%u] name '%s' type '%s' text '%s' not found"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , layer.index
                        , layer.name.c_str()
                        , layer.type.c_str()
                        , layer.name.c_str()
                    );

                    layers_successful = false;

                    continue;
                }
            }
        }

        return layers_successful;
    }
    //////////////////////////////////////////////////////////////////////////
}