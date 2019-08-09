#include "ResourceMovieValidator.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
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

#include <algorithm>

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
    bool ResourceMovieValidator::_validate( const ResourceMoviePtr & _resource )
    {
        float frameDuration = _resource->getFrameDuration();

        if( mt::equal_f_z( frameDuration ) == true )
        {
            LOGGER_ERROR( "'%s' group '%s' m_frameDuration == 0.f"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        uint32_t limitMovieWidth = CONFIG_VALUE( "Limit", "MovieWidth", 16384U );
        uint32_t limitMovieHeight = CONFIG_VALUE( "Limit", "MovieHeight", 16384U );

        const mt::vec2f & size = _resource->getSize();

        uint32_t width = (uint32_t)(size.x + 0.5f);
        uint32_t height = (uint32_t)(size.y + 0.5f);

        if( (width > limitMovieWidth && limitMovieWidth != 0U) || (height > limitMovieHeight && limitMovieHeight != 0U) )
        {
            LOGGER_ERROR( "'%s' group '%s' invalid limit %d:%d size %d:%d"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , limitMovieWidth
                , limitMovieHeight
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
            if( std::find_if( layers.begin(), layers.end(), [layer_parent]( const MovieLayer & _layer )
            {
                return _layer.index == layer_parent;
            } ) == layers.end() )
            {
                LOGGER_ERROR( "'%s' group '%s' layer '%s' index [%d] invalid parent [%d]"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , layer.name.c_str()
                    , layer.index
                    , layer.parent
                );

                return false;
            }
        }

        const FilePath & filePath = _resource->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "'%s' group '%s' don`t have Key Frames Pack Path"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "'%s' group '%s' invalid open file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        ConstString dataflowType = _resource->getDataflowType();

        if( dataflowType.empty() == true )
        {
            dataflowType = CODEC_SERVICE()
                ->findCodecType( filePath );

            if( dataflowType.empty() == true )
            {
                LOGGER_ERROR( "'%s' group '%s' you must determine codec for file '%s'"
                    , _resource->getName().c_str()
                    , _resource->getGroupName().c_str()
                    , _resource->getFilePath().c_str()
                );

                return false;
            }
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        if( dataflow == nullptr )
        {
            return false;
        }

        MovieFramePackInterfacePtr framePack = DATA_SERVICE()
            ->dataflowT<MovieFramePackInterfacePtr>( dataflow, stream, filePath.c_str() );

        if( framePack == nullptr )
        {
            return false;
        }

        bool layers_successful = true;

        for( const MovieLayer & layer : layers )
        {
            if( framePack->hasLayer( layer.index ) == false )
            {
                LOGGER_ERROR( "'%s' group '%s' invalid layer %d '%s' type '%s'"
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
                    LOGGER_ERROR( "'%s' group '%s' invalid layer '%d' SceneEffect should not be threeD"
                        , _resource->getName().c_str()
                        , _resource->getGroupName().c_str()
                        , layer.index
                    );

                    layers_successful = false;

                    continue;
                }
            }

            float MovieImageScale = CONFIG_VALUE( "Limit", "MovieImageScale", 0.75f );

            if( layer.type == STRINGIZE_STRING_LOCAL( "Image" )
                || layer.type == STRINGIZE_STRING_LOCAL( "Animation" )
                || layer.type == STRINGIZE_STRING_LOCAL( "Video" )
                )
            {
                const MovieLayerFrame & layerFrame = framePack->getLayer( layer.index );

                if( layerFrame.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_SCALE || layerFrame.immutable == true )
                {
                    if( ::fabsf( layerFrame.source.scale.x ) < MovieImageScale || ::fabsf( layerFrame.source.scale.y ) < MovieImageScale )
                    {
                        LOGGER_ERROR( "'%s' group '%s' invalid layer '%d':'%s' type '%s' immutable and scale %f:%f (please rescale on graphics editor and re-export)"
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
                        LOGGER_ERROR( "'%s' group '%s' invalid layer '%d':'%s' type '%s' minmax and scale %f:%f (please rescale on graphics editor and re-export)"
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
                    LOGGER_ERROR( "'%s' group '%s' invalid layer '%d':'%s' type '%s' permanently hide"
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
                    LOGGER_ERROR( "'%s' group '%s' invalid layer '%d':'%s' type '%s' text '%s' not found"
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
}