#include "Movie2.h"

#include "Interface/TimelineServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"
#include "Plugins/VideoPlugin/VideoInterface.h"

#include "Movie2Data.h"

#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceTrackMatte.h"
#include "Kernel/Materialable.h"
#include "Kernel/Layer.h"
#include "Kernel/NodeHelper.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/ArrayString.h"
#include "Kernel/AssertionNode.h"
#include "Kernel/NodeCast.h"
#include "Kernel/SurfaceCast.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/StaticString.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdLimits.h"

#include "math/quat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieTransformationPosition( ae_matrix34_t _out, ae_uint32_t _index, ae_float_t _t, ae_userdata_t _userdata )
        {
            MENGINE_UNUSED( _index );
            MENGINE_UNUSED( _t );

            TransformationInterface * transformation = reinterpret_cast<TransformationInterface *>(_userdata);

            const mt::mat4f & wm = transformation->getLocalMatrix();

            _out[3 * 3 + 0] += wm.v3.x;
            _out[3 * 3 + 1] += wm.v3.y;
            _out[3 * 3 + 2] += wm.v3.z;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieTransformation( ae_matrix34_t _out, ae_uint32_t _index, ae_float_t _t, ae_userdata_t _userdata )
        {
            MENGINE_UNUSED( _index );
            MENGINE_UNUSED( _t );

            TransformationInterface * transformation = reinterpret_cast<TransformationInterface *>(_userdata);

            const mt::mat4f & wm = transformation->getLocalMatrix();

            _out[0 * 3 + 0] = wm.v0.x;
            _out[0 * 3 + 1] = wm.v0.y;
            _out[0 * 3 + 2] = wm.v0.z;
            _out[1 * 3 + 0] = wm.v1.x;
            _out[1 * 3 + 1] = wm.v1.y;
            _out[1 * 3 + 2] = wm.v1.z;
            _out[2 * 3 + 0] = wm.v2.x;
            _out[2 * 3 + 1] = wm.v2.y;
            _out[2 * 3 + 2] = wm.v2.z;
            _out[3 * 3 + 0] = wm.v3.x;
            _out[3 * 3 + 1] = wm.v3.y;
            _out[3 * 3 + 2] = wm.v3.z;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionCameraProvider( const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptrptr_t _cameraData, ae_voidptr_t _userdata )
        {
            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );

            ConstString cName = Helper::stringizeString( _callbackData->name );

            Movie2::Camera * oldCamera;
            if( movie2->getCamera_( cName, &oldCamera ) == true )
            {
                *_cameraData = oldCamera;

                return AE_TRUE;
            }

            RenderCameraProjectionPtr renderCameraProjection = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

            renderCameraProjection->setName( cName );

            mt::vec3f cameraTarget;
            cameraTarget.from_f3( _callbackData->target );
            mt::vec3f cameraPosition;
            cameraPosition.from_f3( _callbackData->position );

            mt::vec3f cameraDirection;
            mt::norm_v3_v3( &cameraDirection, cameraTarget - cameraPosition );

            float aspect = _callbackData->width / _callbackData->height;

            renderCameraProjection->setCameraPosition( cameraPosition );
            renderCameraProjection->setCameraDirection( cameraDirection );
            renderCameraProjection->setCameraFOV( _callbackData->fov );
            renderCameraProjection->setCameraAspect( aspect );

            RenderViewportPtr renderViewport = Helper::generateNodeFactorable<RenderViewport>( MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

            renderViewport->setName( cName );

            Viewport viewport;
            viewport.begin.x = 0.f;
            viewport.begin.y = 0.f;

            viewport.end.x = _callbackData->width;
            viewport.end.y = _callbackData->height;

            renderViewport->setViewport( viewport );

            Movie2::Camera * newCamera = movie2->addCamera_( cName, renderCameraProjection, renderViewport );

            *_cameraData = newCamera;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static void movieCompositionCameraDeleter( const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _data )
        {
            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _data );

            ConstString cName = Helper::stringizeString( _callbackData->name );

            movie2->removeCamera_( cName );
        }
        //////////////////////////////////////////////////////////////////////////
        static void movieCompositionCameraUpdate( const aeMovieCameraUpdateCallbackData * _callbackData, ae_voidptr_t _data )
        {
            AE_UNUSED( _data );

            Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(_callbackData->camera_userdata);

            //camera
            mt::vec3f cameraTarget;
            cameraTarget.from_f3( _callbackData->target );
            mt::vec3f cameraPosition;
            cameraPosition.from_f3( _callbackData->position );

            mt::vec3f cameraDirection;
            mt::norm_v3_v3( &cameraDirection, cameraTarget - cameraPosition );

            camera->projection->setCameraPosition( cameraPosition );
            camera->projection->setCameraDirection( cameraDirection );
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getMovieBlendMode( ae_blend_mode_t _blendMode )
        {
            EMaterialBlendMode blendMode = EMB_NORMAL;

            switch( _blendMode )
            {
            case AE_MOVIE_BLEND_ADD:
                blendMode = EMB_ADD;
                break;
            case AE_MOVIE_BLEND_SCREEN:
                blendMode = EMB_SCREEN;
                break;
            case AE_MOVIE_BLEND_MULTIPLY:
                blendMode = EMB_MULTIPLY;
                break;
            default:
                break;
            };

            return blendMode;
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getMovieLayerBlendMode( const aeMovieLayerData * _layer )
        {
            ae_blend_mode_t layerBlendMode = ae_get_movie_layer_data_blend_mode( _layer );

            EMaterialBlendMode blendMode = getMovieBlendMode( layerBlendMode );

            return blendMode;
        }
        //////////////////////////////////////////////////////////////////////////
        static void updateMatrixProxy( Movie2 * _movie2, const Node * _node, ae_bool_t _immutableMatrix, const float * _matrix, ae_bool_t _immutableColor, const ae_color_t & _color, ae_color_channel_t _opacity )
        {
            MENGINE_UNUSED( _movie2 );

            Node * parent = _node->getParent();

            MENGINE_ASSERTION_NODE( parent, MatrixProxy *, "Movie2 '%s' node '%s' type '%s' invalid parent '%s' type '%s' not MatrixProxy"
                , _movie2->getName().c_str()
                , _node->getName().c_str()
                , Helper::getFactorableType( _node ).c_str()
                , parent->getName().c_str()
                , Helper::getFactorableType( parent ).c_str()
            );

            MatrixProxy * matrixProxy = Helper::staticNodeCast<MatrixProxy *>( parent );

            if( _immutableMatrix == AE_FALSE )
            {
                mt::mat4f mp;
                mp.from_f12( _matrix );
                matrixProxy->setProxyMatrix( mp );
            }

            if( _immutableColor == AE_FALSE )
            {
                RenderInterface * render = matrixProxy->getRender();

                render->setLocalColorRGBA( _color.r, _color.g, _color.b, _opacity );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void movieCompositionNodeUpdateAnimation( const aeMovieNodeUpdateCallbackData * _callbackData, AnimationInterface * _animation )
        {
            switch( _callbackData->state )
            {
            case AE_MOVIE_STATE_UPDATE_BEGIN:
                {
                    UpdateContext context;
                    TIMELINE_SERVICE()
                        ->getTotalContext( &context );

                    if( _callbackData->loop == AE_TRUE )
                    {
                        if( _animation->calcTotalLoop() == true )
                        {
                            if( _animation->isPlay() == true )
                            {
                                break;
                            }

                            if( _animation->play( &context ) == 0 )
                            {
                                return;
                            }

                            break;
                        }
                    }

                    float offset = AE_TIME_MILLISECOND( _callbackData->offset );
                    _animation->setTime( offset );

                    if( _animation->play( &context ) == 0 )
                    {
                        return;
                    }

                    if( _callbackData->interrupt != AE_TRUE )
                    {
                        break;
                    }

                    if( _animation->interrupt() == false )
                    {
                        return;
                    }
                }break;
            case AE_MOVIE_STATE_UPDATE_PROCESS:
                {
                }break;
            case AE_MOVIE_STATE_UPDATE_STOP:
                {
                    _animation->stop();
                }break;
            case AE_MOVIE_STATE_UPDATE_END:
                {
                    if( _callbackData->loop != AE_FALSE )
                    {
                        break;
                    }

                    if( _callbackData->interrupt != AE_FALSE )
                    {
                        break;
                    }

                    _animation->stop();
                }break;
            case AE_MOVIE_STATE_UPDATE_PAUSE:
                {
                    _animation->pause();
                }break;
            case AE_MOVIE_STATE_UPDATE_RESUME:
                {
                    UpdateContext context;
                    TIMELINE_SERVICE()
                        ->getTotalContext( &context );

                    _animation->resume( &context );
                }break;
            case AE_MOVIE_STATE_UPDATE_SEEK:
                {
                    _animation->setTime( AE_TIME_MILLISECOND( _callbackData->offset ) );
                }break;
            default:
                {
                }break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionNodeUpdate( const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            Movie2 * movie2 = static_cast<Movie2 *>(_userdata);

            aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer_data );

            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
                {
                    Node * particle = Helper::reinterpretNodeCast<Node *>( _callbackData->element_userdata );

                    Detail::updateMatrixProxy( movie2, particle, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );

                    AnimationInterface * particleAnimation = particle->getAnimation();

                    movieCompositionNodeUpdateAnimation( _callbackData, particleAnimation );
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    Surface * surface = Helper::reinterpretSurfaceCast<Surface *>( _callbackData->element_userdata );

                    AnimationInterface * surfaceAnimation = surface->getAnimation();

                    movieCompositionNodeUpdateAnimation( _callbackData, surfaceAnimation );
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    SurfaceSound * surface = Helper::reinterpretSurfaceCast<SurfaceSound *>( _callbackData->element_userdata );

                    surface->setVolume( _callbackData->volume );

                    AnimationInterface * surfaceAnimation = surface->getAnimation();

                    movieCompositionNodeUpdateAnimation( _callbackData, surfaceAnimation );
                }break;
            case AE_MOVIE_LAYER_TYPE_SLOT:
                {
                    Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( _callbackData->element_userdata );

                    Detail::updateMatrixProxy( movie2, node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
                }break;
            case AE_MOVIE_LAYER_TYPE_SPRITE:
                {
                    ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( _callbackData->element_userdata );

                    Detail::updateMatrixProxy( movie2, node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
                }break;
            case AE_MOVIE_LAYER_TYPE_TEXT:
                {
                    TextField * node = Helper::reinterpretNodeCast<TextField *>( _callbackData->element_userdata );

                    Detail::updateMatrixProxy( movie2, node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
                }break;
            case AE_MOVIE_LAYER_TYPE_SOCKET:
                {
                    HotSpotPolygon * node = Helper::reinterpretNodeCast<HotSpotPolygon *>( _callbackData->element_userdata );

                    Detail::updateMatrixProxy( movie2, node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
                }break;
            default:
                {
                }break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        struct TrackMatteDesc
        {
            mt::mat4f matrix;
            aeMovieRenderMesh mesh;
            ae_track_matte_mode_t mode;
        };
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionTrackMatteProvider( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptrptr_t _trackMatteData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            TrackMatteDesc * desc = Helper::newMemoryT<TrackMatteDesc>( "movie" );

            desc->matrix.from_f12( _callbackData->matrix );
            desc->mesh = *_callbackData->mesh;
            desc->mode = _callbackData->track_matte_mode;

            *_trackMatteData = desc;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionTrackMatteUpdate( const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            switch( _callbackData->state )
            {
            case AE_MOVIE_STATE_UPDATE_BEGIN:
                {
                    TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

                    desc->matrix.from_f12( _callbackData->matrix );
                    desc->mesh = *_callbackData->mesh;
                }break;
            case AE_MOVIE_STATE_UPDATE_PROCESS:
                {
                    TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

                    desc->matrix.from_f12( _callbackData->matrix );
                    desc->mesh = *_callbackData->mesh;
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionTrackMatteDeleter( const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

            Helper::deleteMemoryT( desc, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        struct ShaderParameterDesc
        {
            StaticString<32> uniform;
            ae_uint8_t type;
        };
        //////////////////////////////////////////////////////////////////////////
        struct Movie2ShaderDesc
        {
            RenderMaterialInterfacePtr materials[4][2];
            RenderProgramVariableInterfacePtr programVariable;

            ae_uint32_t flags;
            ae_uint32_t indexOffset;
        };
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionShaderProvider( const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptrptr_t _shaderData, ae_voidptr_t _userdata )
        {
            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );
            AE_UNUSED( movie2 );

            Movie2ShaderDesc * desc = Helper::newMemoryT<Movie2ShaderDesc>( "movie" );

            if( StdString::strlen( _callbackData->description ) == 0 )
            {
                LOGGER_ERROR( "movie '%s' invalid shader '%s' not setup [description]"
                    , movie2->getName().c_str()
                    , _callbackData->name
                );

                return AE_FALSE;
            }

            ArrayString<64> materialNameColorBlend;
            materialNameColorBlend.append( "Color_" );
            materialNameColorBlend.append( _callbackData->description );
            materialNameColorBlend.append( "_Blend" );

            ConstString materialNameColorBlendString = Helper::stringizeString( materialNameColorBlend.c_str() );

            ArrayString<64> materialNameTextureBlend;
            materialNameTextureBlend.append( "Texture_" );
            materialNameTextureBlend.append( _callbackData->description );
            materialNameTextureBlend.append( "_Blend" );

            ConstString materialNameTextureBlendString = Helper::stringizeString( materialNameTextureBlend.c_str() );

            ArrayString<64> materialNameTextureBlendExternalAlpha;
            materialNameTextureBlendExternalAlpha.append( "Texture_" );
            materialNameTextureBlendExternalAlpha.append( _callbackData->description );
            materialNameTextureBlendExternalAlpha.append( "_Blend_ExternalAlpha" );

            ConstString materialNameTextureBlendExternalAlphaString = Helper::stringizeString( materialNameTextureBlendExternalAlpha.c_str() );

            ae_userdata_t resourceUserdata = ae_get_movie_layer_data_resource_userdata( _callbackData->layer_data );

            aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer_data );

            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_SOLID:
                {
                    desc->materials[EMB_NORMAL][0] = Helper::makeSolidMaterial( materialNameColorBlendString, EMB_NORMAL, false, MENGINE_DOCUMENT_FORWARD_PTR( movie2 ) );
                    desc->materials[EMB_NORMAL][1] = nullptr;
                } break;
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

                    ResourceImage * resourceImage = imageDesc->resourceImage;

                    desc->materials[EMB_NORMAL][0] = Helper::makeImageMaterial( resourceImage, materialNameTextureBlendString, EMB_NORMAL, false, false, MENGINE_DOCUMENT_FORWARD_PTR( movie2 ) );
                    desc->materials[EMB_NORMAL][1] = Helper::makeImageMaterial( resourceImage, materialNameTextureBlendExternalAlphaString, EMB_NORMAL, false, false, MENGINE_DOCUMENT_FORWARD_PTR( movie2 ) );
                } break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    ResourceImage * resourceImage = reinterpret_cast<ResourceImage *>(resourceUserdata);

                    desc->materials[EMB_NORMAL][0] = Helper::makeImageMaterial( resourceImage, materialNameTextureBlendString, EMB_NORMAL, false, false, MENGINE_DOCUMENT_FORWARD_PTR( movie2 ) );
                    desc->materials[EMB_NORMAL][1] = Helper::makeImageMaterial( resourceImage, materialNameTextureBlendExternalAlphaString, EMB_NORMAL, false, false, MENGINE_DOCUMENT_FORWARD_PTR( movie2 ) );
                } break;
            default:
                {
                    LOGGER_ERROR( "movie '%s' invalid shader '%s' not support layer type %d"
                        , movie2->getName().c_str()
                        , _callbackData->name
                        , layerType
                    );

                    return AE_FALSE;
                }break;
            }

            desc->flags = _callbackData->flags;
            desc->indexOffset = 0;

            RenderProgramVariableInterfacePtr programVariable = RENDER_SYSTEM()
                ->createProgramVariableStatic( 0, desc->indexOffset + _callbackData->parameter_count, MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

            if( (_callbackData->flags & AE_MOVIE_EXTENSION_SHADER_FLAG_UVSL) == AE_MOVIE_EXTENSION_SHADER_FLAG_UVSL )
            {
                float shaderUvsl[4] = {0.f, 0.f, 1.f, 1.f};
                programVariable->setPixelVariables( "uvsl", desc->indexOffset + 0, shaderUvsl, 4, 1 );

                desc->indexOffset += 1;
            }

            for( ae_uint32_t index = 0; index != _callbackData->parameter_count; ++index )
            {
                ShaderParameterDesc parameter;

                ae_string_t uniform = _callbackData->parameter_uniforms[index];
                parameter.uniform.assign( uniform );
                parameter.type = _callbackData->parameter_types[index];

                float shaderValues[4] = {0.f, 0.f, 0.f, 0.f};

                switch( parameter.type )
                {
                case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
                    {
                        ae_float_t value = _callbackData->parameter_values[index];

                        shaderValues[0] = value;

                        programVariable->setPixelVariables( parameter.uniform.c_str(), desc->indexOffset + index, shaderValues, 1, 1 );
                    }break;
                case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
                    {
                        ae_float_t value = _callbackData->parameter_values[index];

                        shaderValues[0] = value;

                        programVariable->setPixelVariables( parameter.uniform.c_str(), desc->indexOffset + index, shaderValues, 1, 1 );
                    }break;
                case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
                    {
                        ae_color_t colorValue = _callbackData->parameter_colors[index];

                        shaderValues[0] = colorValue.r;
                        shaderValues[1] = colorValue.g;
                        shaderValues[2] = colorValue.b;
                        shaderValues[3] = 1.f;

                        programVariable->setPixelVariables( parameter.uniform.c_str(), desc->indexOffset + index, shaderValues, 4, 1 );
                    }break;
                default:
                    return AE_FALSE;
                }
            }

            desc->programVariable = programVariable;

            *_shaderData = desc;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionShaderDeleter( const aeMovieShaderDeleterCallbackData * _callbackData, ae_userdata_t _userdata )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _userdata );

            Movie2ShaderDesc * desc = reinterpret_cast<Movie2ShaderDesc *>(_callbackData->element_userdata);

            desc->materials[EMB_NORMAL][0] = nullptr;
            desc->materials[EMB_NORMAL][1] = nullptr;

            desc->programVariable = nullptr;

            Helper::deleteMemoryT( desc, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionShaderPropertyUpdate( const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _userdata );

            Movie2ShaderDesc * desc = reinterpret_cast<Movie2ShaderDesc *>(_callbackData->element_userdata);

            const RenderProgramVariableInterfacePtr & programVariable = desc->programVariable;

            float shaderValues[4] = {0.f, 0.f, 0.f, 0.f};

            switch( _callbackData->type )
            {
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
                {
                    shaderValues[0] = _callbackData->value;

                    programVariable->updatePixelVariables( desc->indexOffset + _callbackData->index, shaderValues, 1, 1 );
                }break;
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
                {
                    shaderValues[0] = _callbackData->value;

                    programVariable->updatePixelVariables( desc->indexOffset + _callbackData->index, shaderValues, 1, 1 );
                }break;
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
                {
                    shaderValues[0] = _callbackData->color.r;
                    shaderValues[1] = _callbackData->color.g;
                    shaderValues[2] = _callbackData->color.b;
                    shaderValues[3] = 1.f;

                    programVariable->updatePixelVariables( desc->indexOffset + _callbackData->index, shaderValues, 4, 1 );
                }break;
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_TIME:
                {
                    shaderValues[0] = _callbackData->value;

                    programVariable->updatePixelVariables( desc->indexOffset + _callbackData->index, shaderValues, 1, 1 );
                }break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionEvent( const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _userdata );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCompositionState( const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );

            switch( _callbackData->state )
            {
            case AE_MOVIE_COMPOSITION_PLAY:
            case AE_MOVIE_COMPOSITION_STOP:
            case AE_MOVIE_COMPOSITION_PAUSE:
            case AE_MOVIE_COMPOSITION_RESUME:
            case AE_MOVIE_COMPOSITION_LOOP_END:
                {
                    //Empty
                }break;
            case AE_MOVIE_COMPOSITION_INTERRUPT:
                {
                    if( movie2->interruptElements_() == false )
                    {
                        LOGGER_ERROR( "movie2 '%s' invalid interrupt elements"
                            , movie2->getName().c_str()
                        );
                    }
                }break;
            case AE_MOVIE_COMPOSITION_LOOP_CONTINUOUS:
                {
                }break;
            case AE_MOVIE_COMPOSITION_END:
                {
                    AnimationInterface * animation = movie2->getAnimation();

                    animation->end();
                }break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionExtraInterrupt( const aeMovieCompositionExtraInterruptCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _callbackData );

            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );

            if( movie2->checkInterruptElement_() == false )
            {
                return AE_FALSE;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieSceneEffectProvider( const aeMovieCompositionSceneEffectProviderCallbackData * _callbackData, ae_voidptrptr_t _sceneEffectData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _sceneEffectData );

            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );

            Layer * parentLayer = Helper::findParentNodeT<Layer *>( movie2 );

            TransformationInterface * parentLayerTransformation = parentLayer->getTransformation();

            mt::vec3f anchorPoint;
            anchorPoint.from_f2( _callbackData->anchor_point, 0.f );
            parentLayerTransformation->setLocalOrigin( anchorPoint );

            mt::vec3f position;
            position.from_f2( _callbackData->position, 0.f );
            parentLayerTransformation->setLocalPosition( position );

            mt::vec3f scale;
            scale.from_f2( _callbackData->scale, 1.f );
            parentLayerTransformation->setLocalScale( scale );

            mt::quatf q;
            q.x = 0.f;
            q.y = 0.f;
            q.z = _callbackData->quaternion[0];
            q.w = _callbackData->quaternion[1];
            float angle = quatzw_to_angle( q );

            parentLayerTransformation->setLocalOrientationX( angle );

            Color color( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

            RenderInterface * parentLayerRender = parentLayer->getRender();
            parentLayerRender->setLocalColor( color );

            *_sceneEffectData = parentLayer;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieSceneEffectUpdate( const aeMovieCompositionSceneEffectUpdateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            Layer * parentLayer = Helper::reinterpretNodeCast<Layer *>( _callbackData->scene_effect_userdata );

            TransformationInterface * parentLayerTransformation = parentLayer->getTransformation();

            mt::vec3f anchorPoint;
            anchorPoint.from_f2( _callbackData->anchor_point, 0.f );
            parentLayerTransformation->setLocalOrigin( anchorPoint );

            mt::vec3f position;
            position.from_f2( _callbackData->position, 0.f );
            parentLayerTransformation->setLocalPosition( position );

            mt::vec3f scale;
            scale.from_f2( _callbackData->scale, 1.f );
            parentLayerTransformation->setLocalScale( scale );

            mt::quatf q;
            q.x = 0.f;
            q.y = 0.f;
            q.z = _callbackData->quaternion[0];
            q.w = _callbackData->quaternion[1];
            float angle = quatzw_to_angle( q );

            parentLayerTransformation->setLocalOrientationX( angle );

            Color color( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

            RenderInterface * parentLayerRender = parentLayer->getRender();
            parentLayerRender->setLocalColor( color );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieSubcompositionProvider( const aeMovieSubCompositionProviderCallbackData * _callbackData, ae_voidptrptr_t _subcompositionData, ae_voidptr_t _userdata )
        {
            Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _userdata );

            const aeMovieLayerData * layerData = _callbackData->layer_data;

            const ae_char_t * layerName = ae_get_movie_layer_data_name( layerData );

            ConstString cName = Helper::stringizeString( layerName );

            const Movie2SubCompositionPtr & subcomposition = movie2->getSubComposition( cName );

            MENGINE_ASSERTION_MEMORY_PANIC( subcomposition, "movie2 '%s' not found subcomposition '%s'"
                , movie2->getName().c_str()
                , cName.c_str()
            );

            *_subcompositionData = subcomposition.get();

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieSubcompositionDeleter( const aeMovieSubCompositionDeleterCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _userdata );

            //Empty
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieSubcompositionState( const aeMovieSubCompositionStateCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            Movie2SubComposition * m2sc = reinterpret_cast<Movie2SubComposition *>(_callbackData->subcomposition_userdata);

            if( _callbackData->state == AE_MOVIE_COMPOSITION_END )
            {
                AnimationInterface * animation = m2sc->getAnimation();

                animation->end();
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::Movie2()
        : m_compositionRevision( 0 )
        , m_composition( nullptr )
        , m_duration( 0.f )
        , m_frameDuration( 0.f )
        , m_preCompileTime( 0.f )
        , m_hasBounds( false )
        , m_bounds( {0.f, 0.f}, {0.f, 0.f} )
        , m_compositionAlive( AE_FALSE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::~Movie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setResourceMovie2( const ResourcePtr & _resource )
    {
        ResourceMovie2Ptr resourceMovie2 = ResourceMovie2Ptr::from( _resource );

        if( m_resourceMovie2 == resourceMovie2 )
        {
            return;
        }

        bool surfacesActivated = this->isActivate();

        if( surfacesActivated == true )
        {
            this->deactivateSurfaces_();
        }

        this->recompile( [this, &resourceMovie2]()
        {
            m_resourceMovie2 = resourceMovie2;

            if( m_resourceMovie2 == nullptr )
            {
                return false;
            }

            return true;
        } );

        if( surfacesActivated == false )
        {
            return;
        }

        if( this->isActivate() == false )
        {
            return;
        }

        this->activateSurfaces_();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Movie2::getResourceMovie2() const
    {
        return m_resourceMovie2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setCompositionName( const ConstString & _compositionName )
    {
        if( m_compositionName == _compositionName )
        {
            return true;
        }

        if( m_resourceMovie2 == nullptr )
        {
            m_compositionName = _compositionName;

            return true;
        }

        bool surfacesActivated = this->isActivate();

        if( surfacesActivated == true )
        {
            this->deactivateSurfaces_();
        }

        bool successful = true;
        this->recompile( [this, &successful, _compositionName]()
        {
            if( m_compositionName.empty() == false )
            {
                this->disposeCompositionLayers_();
            }

            m_compositionName = _compositionName;

            if( _compositionName.empty() == true )
            {
                return true;
            }

            if( this->createCompositionLayers_() == false )
            {
                successful = false;

                return true;
            }

            return true;
        } );

        if( surfacesActivated == false )
        {
            return successful;
        }

        if( this->isActivate() == false )
        {
            return successful;
        }

        this->activateSurfaces_();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2::getCompositionName() const
    {
        return m_compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieComposition * Movie2::getComposition() const
    {
        return m_composition;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setMotionParameter( aeMovieMotionParameter _parameter, float _value )
    {
        if( m_composition == nullptr )
        {
            return false;
        }

        if( ae_set_movie_motion_parameter( m_composition, _parameter, _value ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::getMotionParameter( aeMovieMotionParameter _parameter ) const
    {
        if( m_composition == nullptr )
        {
            return 0.f;
        }

        float value = 0.f;
        ae_get_movie_motion_parameter( m_composition, _parameter, &value );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::getMotionParameters( VectorConstString * const _names ) const
    {
        if( m_composition == nullptr )
        {
            return;
        }

        const Movie2DataInterfacePtr & movieData = m_resourceMovie2->getData();
        const aeMovieData * data = movieData->getMovieData();
        if( ae_is_movie_motion_data( data ) == AE_FALSE )
        {
            return;
        }

        for( uint32_t index = 0; index != AE_MOVIE_MOTION_PARAMETER_COUNT; ++index )
        {
            aeMovieMotionParameter parameter = static_cast<aeMovieMotionParameter>(index);
            const Char * parameterName = ae_get_movie_motion_parameter_name( parameter );
            const ConstString & name = Helper::stringizeString( parameterName );
            _names->emplace_back( name );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setMotionRegister( uint32_t _index, float _value )
    {
        if( m_composition == nullptr )
        {
            return false;
        }

        if( ae_set_movie_motion_register( m_composition, _index, _value ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::getMotionRegister( uint32_t _index ) const
    {
        if( m_composition == nullptr )
        {
            return 0.f;
        }

        float value = 0.f;
        ae_get_movie_motion_register( m_composition, _index, &value );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::getMotionRegisters( VectorConstString * const _names ) const
    {
        if( m_composition == nullptr )
        {
            return;
        }

        const Movie2DataInterfacePtr & movieData = m_resourceMovie2->getData();
        const aeMovieData * data = movieData->getMovieData();
        uint32_t count = ae_get_movie_motion_register_count( data );

        for( uint32_t index = 0; index != count; ++index )
        {
            const Char * registerName = ae_get_movie_motion_register_name( data, index );
            const ConstString & name = Helper::stringizeString( registerName );
            _names->emplace_back( name );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setTextAliasEnvironment( const ConstString & _aliasEnvironment )
    {
        if( m_aliasEnvironment == _aliasEnvironment )
        {
            return;
        }

        m_aliasEnvironment = _aliasEnvironment;

        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            text->setTextAliasEnvironment( m_aliasEnvironment );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2::getTextAliasEnvironment() const
    {
        return m_aliasEnvironment;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setWorkAreaFromEvent( const ConstString & _eventName )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "name '%s' invalid setup '%s' not compile"
            , this->getName().c_str()
            , _eventName.c_str()
        );

        float inTime;
        float outTime;
        if( ae_get_movie_composition_node_in_out_time( m_composition, _eventName.c_str(), AE_MOVIE_LAYER_TYPE_EVENT, &inTime, &outTime ) == AE_FALSE )
        {
            return false;
        }

        ae_set_movie_composition_work_area( m_composition, inTime, outTime );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeWorkArea()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' not compile"
            , this->getName().c_str()
        );

        ae_remove_movie_composition_work_area( m_composition );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasCompositionBounds() const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        return m_hasBounds;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f & Movie2::getCompositionBounds() const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        return m_bounds;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSubComposition( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        bool result = m_subCompositions.exist( _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SubCompositionPtr & Movie2::getSubComposition( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        const Movie2SubCompositionPtr & subComposition = m_subCompositions.find( _name );

        return subComposition;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachSubComposition( const LambdaSubCompositions & _lambda ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        for( const HashtableSubCompositions::value_type & element : m_subCompositions )
        {
            const Movie2SubCompositionPtr & value = element.element;

            _lambda( value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasMovieLayers( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' not compile (layer '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        if( ae_has_movie_composition_node_any( m_composition, _name.c_str() ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setEnableMovieLayers( const ConstString & _name, bool _enable )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' not compile (layers '%s' enable: %d)"
            , this->getName().c_str()
            , _name.c_str()
            , _enable
        );

#if defined(MENGINE_DEBUG)
        if( ae_has_movie_composition_node_any( m_composition, _name.c_str() ) == AE_FALSE )
        {
            LOGGER_ERROR( "movie2 '%s' layer '%s' not found"
                , this->getName().c_str()
                , _name.c_str()
            );

            return;
        }
#endif

        ae_set_movie_composition_nodes_enable_any( m_composition, _name.c_str(), _enable ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setExtraOpacityMovieLayers( const ConstString & _name, float _opacity )
    {
        MENGINE_ASSERTION_FATAL( _opacity >= 0.f && _opacity <= 1.f, "invalid opacity [%.2f] (0..1)", _opacity );

        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' not compile (layers '%s' opacity: %f)"
            , this->getName().c_str()
            , _name.c_str()
            , _opacity
        );

#if defined(MENGINE_DEBUG)
        if( ae_has_movie_composition_node_any( m_composition, _name.c_str() ) == AE_FALSE )
        {
            LOGGER_ERROR( "movie2 '%s' layer '%s' not found"
                , this->getName().c_str()
                , _name.c_str()
            );

            return;
        }
#endif

        ae_set_movie_composition_nodes_extra_opacity_any( m_composition, _name.c_str(), _opacity );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setExtraTransformation( const ConstString & _name, const TransformationInterfacePtr & _transformation, bool _onlyPosition )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' invalid set '%s' not compile"
            , this->getName().c_str()
            , _name.c_str()
        );

        MENGINE_ASSERTION_MEMORY_PANIC( _transformation, "movie2 '%s' invalid set '%s' null extra transformation"
            , this->getName().c_str()
            , _name.c_str()
        );

        MENGINE_ASSERTION_FATAL( ae_has_movie_composition_node( m_composition, _name.c_str(), AE_MOVIE_LAYER_TYPE_NONE ) == AE_TRUE, "movie2 '%s' invalid set '%s' not found"
            , this->getName().c_str()
            , _name.c_str()
        );

        TransformationInterface * t = _transformation.acquire();

        if( _onlyPosition == true )
        {
            ae_set_movie_composition_node_extra_transformation( m_composition, _name.c_str(), &Detail::movieTransformationPosition, t );
        }
        else
        {
            ae_set_movie_composition_node_extra_transformation( m_composition, _name.c_str(), &Detail::movieTransformation, t );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeExtraTransformation( const ConstString & _name )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' invalid set '%s' not compile"
            , this->getName().c_str()
            , _name.c_str()
        );

        MENGINE_ASSERTION_FATAL( ae_has_movie_composition_node( m_composition, _name.c_str(), AE_MOVIE_LAYER_TYPE_NONE ) == AE_TRUE, "movie2 '%s' invalid set '%s' not found"
            , this->getName().c_str()
            , _name.c_str()
        );

        TransformationInterface * t = (TransformationInterface *)ae_remove_movie_composition_node_extra_transformation( m_composition, _name.c_str() );

        MENGINE_ASSERTION_MEMORY_PANIC( t, "movie2 '%s' invalid remove '%s' transformation not set"
            , this->getName().c_str()
            , _name.c_str()
        );

        TransformationInterfacePtr::release( t );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::getWorldBoundingBox( mt::box2f * const _bb ) const
    {
        if( this->isCompile() == false )
        {
            return false;
        }

        bool successful = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::box2f bbwm;
        mt::box2_insideout( &bbwm );

        ae_uint32_t computeMovieMeshIterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &computeMovieMeshIterator, &mesh ) == AE_TRUE )
        {
            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        RenderInterface * render = node->getRender();

                        const mt::box2f * bb = render->getBoundingBox();

                        if( bb != nullptr )
                        {
                            mt::box2_merge( &bbwm, *bb );

                            successful = true;
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        const RenderInterface * render = node->getRender();

                        const mt::box2f * bb = render->getBoundingBox();

                        if( bb != nullptr )
                        {
                            mt::box2_merge( &bbwm, *bb );

                            successful = true;
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        //Node * node = reinterpret_node_cast<Node *>(mesh.element_userdata);

                        //const mt::box2f * bb = node->getBoundingBox();

                        //if( bb == nullptr )
                        //{
                        //    mt::merge_box( bb, *bb );
                        //}
                    }break;
                case AE_MOVIE_LAYER_TYPE_SHAPE_POLYGON:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            mt::vec3f p;
                            p.from_f3( mesh.position[index] );

                            mt::vec2f pwm;
                            mt::mul_v2_v3_m4( &pwm, p, wm );

                            mt::box2_add_internal_point( &bbwm, pwm );
                        }

                        successful = true;
                    }break;
                default:
                    break;
                }
            }
            else
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            mt::vec3f p;
                            p.from_f3( mesh.position[index] );

                            mt::vec2f pwm;
                            mt::mul_v2_v3_m4( &pwm, p, wm );

                            mt::box2_add_internal_point( &bbwm, pwm );
                        }

                        successful = true;
                    }break;
                default:
                    break;
                }
            }
        }

        *_bb = bbwm;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachRenderSlots( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const LambdaMovieRenderSlot & _lambda ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "movie2 '%s' not compile"
            , this->getName().c_str()
        );

        const mt::mat4f & wm = this->getWorldMatrix();

        ae_userdata_t compositionCameraUserdata = ae_get_movie_composition_camera_userdata( m_composition );
        AE_UNUSED( compositionCameraUserdata );

        ae_uint32_t computeMovieMeshIterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &computeMovieMeshIterator, &mesh ) == AE_TRUE )
        {
            RenderContext context;

            context = *_context;

            if( mesh.camera_userdata != nullptr )
            {
                Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(mesh.camera_userdata);

                context.camera = camera->projection.get();
                context.viewport = camera->viewport.get();
            }

            Movie2ScissorPtr scissor;
            if( mesh.viewport != nullptr )
            {
                scissor = Helper::makeFactorableUnique<Movie2Scissor>( MENGINE_DOCUMENT_FACTORABLE );

                scissor->setScissorViewport( wm, mesh.viewport );

                context.scissor = scissor.get();
            }
            else
            {
                context.scissor = _context->scissor;
            }

            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( mesh.element_userdata );

                        _lambda( node, _renderPipeline, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                        HotSpotPolygon * node = Helper::reinterpretNodeCast<HotSpotPolygon *>( mesh.element_userdata );

                        _lambda( node, _renderPipeline, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        _lambda( node, _renderPipeline, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        _lambda( node, _renderPipeline, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        Node * node = Helper::reinterpretNodeCast<Node *>( mesh.element_userdata );

                        _lambda( node, _renderPipeline, &context );
                    }break;
                default:
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ShapeQuadFixedPtr & Movie2::findSprite( const ConstString & _name ) const
    {
        for( const HashtableSprites::value_type & value : m_sprites )
        {
            const ShapeQuadFixedPtr & sprite = value.element;

            if( sprite->getName() != _name )
            {
                continue;
            }

            return sprite;
        }

        const ShapeQuadFixedPtr & sprite = ShapeQuadFixedPtr::none();

        return sprite;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSprite( const ConstString & _name ) const
    {
        for( const HashtableSprites::value_type & value : m_sprites )
        {
            const ShapeQuadFixedPtr & sprite = value.element;

            if( sprite->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Movie2::findParticle( const ConstString & _name ) const
    {
        for( const HashtableAstralaxEmitters::value_type & value : m_astralaxEmitters )
        {
            const NodePtr & particle = value.element;

            if( particle->getName() != _name )
            {
                continue;
            }

            return particle;
        }

        const NodePtr & particle = NodePtr::none();

        return particle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasParticle( const ConstString & _name ) const
    {
        for( const HashtableAstralaxEmitters::value_type & value : m_astralaxEmitters )
        {
            const NodePtr & particle = value.element;

            if( particle->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SlotPtr & Movie2::findSlot( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (slot '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableSlots::value_type & value : m_slots )
        {
            const Movie2SlotPtr & slot = value.element;

            if( slot->getName() != _name )
            {
                continue;
            }

            return slot;
        }

        const Movie2SlotPtr & slot = Movie2SlotPtr::none();

        return slot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSlot( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (slot '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableSlots::value_type & value : m_slots )
        {
            const Movie2SlotPtr & slot = value.element;

            if( slot->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachSlots( const LambdaNodes & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        for( const HashtableSlots::value_type & value : m_slots )
        {
            uint32_t index = value.key;
            const Movie2SlotPtr & node = value.element;

            _lambda( this, index, node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const HotSpotPolygonPtr & Movie2::findSocket( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (socket '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & socket = value.element;

            if( socket->getName() != _name )
            {
                continue;
            }

            return socket;
        }

        const HotSpotPolygonPtr & socket = HotSpotPolygonPtr::none();

        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSocket( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (socket '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & socket = value.element;

            if( socket->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachSockets( const LambdaNodes & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        for( const HashtableSockets::value_type & value : m_sockets )
        {
            uint32_t key = value.key;
            const HotSpotPolygonPtr & hotspot = value.element;

            _lambda( this, key, hotspot );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFieldPtr & Movie2::findText( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (text '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            if( text->getName() != _name )
            {
                continue;
            }

            return text;
        }

        const TextFieldPtr & text = TextFieldPtr::none();

        return text;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasText( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name (text '%s')"
            , this->getName().c_str()
            , _name.c_str()
        );

        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            if( text->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachTexts( const LambdaNodes & _lambda )
    {
        MENGINE_ASSERTION_FATAL( m_compositionName.empty() == false, "movie2 '%s' not setup composition name"
            , this->getName().c_str()
        );

        for( const HashtableTexts::value_type & value : m_texts )
        {
            uint32_t index = value.key;
            const TextFieldPtr & node = value.element;

            _lambda( this, index, node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::Camera * Movie2::addCamera_( const ConstString & _name, const RenderCameraProjectionPtr & _projection, const RenderViewportPtr & _viewport )
    {
        this->addChild( _projection );
        this->addChild( _viewport );

        Camera c;
        c.projection = _projection;
        c.viewport = _viewport;

        MapCameras::iterator itFound = m_cameras.emplace( _name, c ).first;

        Camera * newCamera = &itFound->second;

        return newCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::removeCamera_( const ConstString & _name )
    {
        MapCameras::iterator itFound = m_cameras.find( _name );

        if( itFound == m_cameras.end() )
        {
            return false;
        }

        Camera & c = itFound->second;

        c.projection->dispose();
        c.projection = nullptr;

        c.viewport->dispose();
        c.viewport = nullptr;

        m_cameras.erase( itFound );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasCamera_( const ConstString & _name ) const
    {
        MapCameras::const_iterator itFound = m_cameras.find( _name );

        if( itFound == m_cameras.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::getCamera_( const ConstString & _name, Camera ** const _camera )
    {
        MapCameras::iterator itFound = m_cameras.find( _name );

        if( itFound == m_cameras.end() )
        {
            return false;
        }

        Camera & camera = itFound->second;

        *_camera = &camera;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::interruptElements_()
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation == nullptr )
            {
                continue;
            }

            if( animation->isPlay() == false )
            {
                continue;
            }

            if( animation->isInterrupt() == true )
            {
                return false;
            }

            if( animation->interrupt() == false )
            {
                return false;
            }
        }

        for( const HashtableAstralaxEmitters::value_type & value : m_astralaxEmitters )
        {
            const NodePtr & astralaxEmitter = value.element;

            AnimationInterface * animation = astralaxEmitter->getAnimation();

            if( animation == nullptr )
            {
                continue;
            }

            if( animation->isPlay() == false )
            {
                continue;
            }

            if( animation->isInterrupt() == true )
            {
                return false;
            }

            if( animation->interrupt() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::checkInterruptElement_() const
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation != nullptr )
            {
                if( animation->isInterrupt() == true )
                {
                    return false;
                }
            }
        }

        for( const HashtableAstralaxEmitters::value_type & value : m_astralaxEmitters )
        {
            const NodePtr & astralaxEmitter = value.element;

            AnimationInterface * animation = astralaxEmitter->getAnimation();

            if( animation != nullptr )
            {
                if( animation->isInterrupt() == true )
                {
                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_play( UniqueId _playId, float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_play_movie_composition( m_composition, -1.f );

        EVENTABLE_METHOD( EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_restart( UniqueId _playId, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_pause( UniqueId _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        ae_pause_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_resume( UniqueId _playId, float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        ae_resume_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _playId, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_stop( UniqueId _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_stop_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_end( UniqueId _playId )
    {
        uint32_t compositionRevision = m_compositionRevision;

        if( m_composition != nullptr )
        {
            ae_finish_movie_motion( m_composition );
            this->flushMovieMarkers_();
        }

        if( compositionRevision != m_compositionRevision )
        {
            return;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_interrupt( UniqueId _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_interrupt_movie_composition( m_composition, AE_FALSE );

        EVENTABLE_METHOD( EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceMovie2, "name '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceMovie2->compile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceMovie2->getName().c_str()
            );

            return false;
        }

        const aeMovieCompositionData * compositionData = m_resourceMovie2->getCompositionData( m_compositionName );

        MENGINE_ASSERTION_MEMORY_PANIC( compositionData, "name '%s' resource '%s' not found composition '%s'"
            , this->getName().c_str()
            , m_resourceMovie2->getName().c_str()
            , m_compositionName.c_str()
        );

        const Movie2DataInterfacePtr & data = m_resourceMovie2->getData();

        if( data->acquireCompositionData( compositionData ) == false )
        {
            LOGGER_ERROR( "movie2 '%s' resource '%s' not acquire composition"
                , this->getName().c_str()
                , m_resourceMovie2->getName().c_str()
            );

            return false;
        }

        aeMovieCompositionProviders providers;
        ae_initialize_movie_composition_providers( &providers );

        providers.camera_provider = &Detail::movieCompositionCameraProvider;
        providers.camera_deleter = &Detail::movieCompositionCameraDeleter;
        providers.camera_update = &Detail::movieCompositionCameraUpdate;

        providers.node_provider = &Movie2::movieCompositionNodeProvider_;
        providers.node_deleter = &Movie2::movieCompositionNodeDeleter_;
        providers.node_update = &Detail::movieCompositionNodeUpdate;

        providers.track_matte_provider = &Detail::movieCompositionTrackMatteProvider;
        providers.track_matte_deleter = &Detail::movieCompositionTrackMatteDeleter;
        providers.track_matte_update = &Detail::movieCompositionTrackMatteUpdate;

        providers.shader_provider = &Detail::movieCompositionShaderProvider;
        providers.shader_deleter = &Detail::movieCompositionShaderDeleter;
        providers.shader_property_update = &Detail::movieCompositionShaderPropertyUpdate;

        providers.composition_event = &Detail::movieCompositionEvent;
        providers.composition_state = &Detail::movieCompositionState;
        providers.composition_extra_interrupt = &Detail::movieCompositionExtraInterrupt;

        providers.scene_effect_provider = &Detail::movieSceneEffectProvider;
        providers.scene_effect_update = &Detail::movieSceneEffectUpdate;

        providers.subcomposition_provider = &Detail::movieSubcompositionProvider;
        providers.subcomposition_deleter = &Detail::movieSubcompositionDeleter;
        providers.subcomposition_state = &Detail::movieSubcompositionState;

        const aeMovieComposition * composition = ae_create_movie_composition( compositionData, AE_TRUE, &providers, this );

        MENGINE_ASSERTION_MEMORY_PANIC( composition, "name '%s' resource '%s' invalid create composition '%s'"
            , this->getName().c_str()
            , m_resourceMovie2->getName().c_str()
            , m_compositionName.c_str()
        );

        m_composition = composition;

        const aeMovieData * movieData = data->getMovieData();

        if( ae_is_movie_motion_data( movieData ) == AE_TRUE )
        {
            float duration = ae_get_movie_composition_data_duration( compositionData );
            float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );

            m_duration = AE_TIME_MILLISECOND( duration );
            m_frameDuration = AE_TIME_MILLISECOND( frameDuration );
        }

        ++m_compositionRevision;
        ae_set_movie_marker_callback( m_composition, &Movie2::movieMarkerCallback_, this );

        aeMovieCompositionRenderInfo info;
        ae_calculate_movie_composition_render_info( m_composition, &info );

        if( info.max_vertex_count != 0 )
        {
            m_vertices.resize( info.max_vertex_count );
        }

        if( info.max_index_count != 0 )
        {
            m_indices.resize( info.max_index_count );
        }

        bool loop = this->isLoop();

        ae_set_movie_composition_loop( m_composition, loop ? AE_TRUE : AE_FALSE );

        for( const HashtableSubCompositions::value_type & value : m_subCompositions )
        {
            const Movie2SubCompositionPtr & subComposition = value.element;

            if( subComposition->initialize( m_composition ) == false )
            {
                return false;
            }
        }

        bool play = this->isPlay();

        if( play == true )
        {
            if( m_preCompileTime != -1.f )
            {
                ae_play_movie_composition( m_composition, m_preCompileTime * 0.001f );
            }
            else
            {
                float duration = ae_get_movie_composition_data_duration( compositionData );
                float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );

                ae_play_movie_composition( m_composition, duration - frameDuration );
            }
        }
        else
        {
            if( m_preCompileTime != -1.f )
            {
                ae_set_movie_composition_time( m_composition, m_preCompileTime * 0.001f );
            }
            else
            {
                float duration = ae_get_movie_composition_data_duration( compositionData );
                float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );

                ae_set_movie_composition_time( m_composition, duration - frameDuration );
            }
        }

        m_preCompileTime = 0.f;

        m_compositionAlive = AE_TRUE;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_release()
    {
        ++m_compositionRevision;
        m_pendingMarkers.clear();

        m_compositionAlive = AE_FALSE;

        const Movie2DataInterfacePtr & data = m_resourceMovie2->getData();

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_composition_data( m_composition );

        data->releaseCompositionData( compositionData );

        ae_delete_movie_composition( m_composition );
        m_composition = nullptr;

        m_cameras.clear();

        for( const HashtableSubCompositions::value_type & value : m_subCompositions )
        {
            const Movie2SubCompositionPtr & subComposition = value.element;

            subComposition->finalize();
        }

        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation != nullptr )
            {
                animation->setRelationAnimation( nullptr );
                animation->setRelationIncessantly( false );
            }

            surface->release();
        }

        m_surfaces.clear();

        m_resourceMovie2->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_activate()
    {
        ae_vector3_t anchorPoint;
        if( ae_get_movie_composition_anchor_point( m_composition, anchorPoint ) == AE_TRUE )
        {
            mt::vec3f origin;
            origin.from_f3( anchorPoint );

            this->setLocalOrigin( origin );
        }

        this->activateSurfaces_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_deactivate()
    {
        this->deactivateSurfaces_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        bool autoPlay = this->isAutoPlay();

        if( autoPlay == true )
        {
            UpdateContext context;
            TIMELINE_SERVICE()
                ->getTotalContext( &context );

            if( this->play( &context ) == 0 )
            {
                LOGGER_ERROR( "movie2 '%s' resource '%s' auto play return 0"
                    , this->getName().c_str()
                    , this->m_resourceMovie2->getName().c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        ae_set_movie_composition_loop( m_composition, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            m_preCompileTime = _time;

            return;
        }

        ae_set_movie_composition_time( m_composition, _time * 0.001f );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::_getTime() const
    {
        if( this->isCompile() == false )
        {
            return m_preCompileTime;
        }

        float time = ae_get_movie_composition_time( m_composition );

        float milliseconds = AE_TIME_MILLISECOND( time );

        return milliseconds;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::_getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setFirstFrame()
    {
        this->setTime( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLastFrame()
    {
        if( this->isCompile() == false )
        {
            m_preCompileTime = -1.f;

            return;
        }

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_composition_data( m_composition );

        float duration = ae_get_movie_composition_data_duration( compositionData );
        float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );

        this->setTime( AE_TIME_MILLISECOND( (duration - frameDuration) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::update( const UpdateContext * _context )
    {
        if( m_composition == nullptr )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        ae_update_movie_composition( m_composition, totalTime * 0.001f, &m_compositionAlive );
        this->flushMovieMarkers_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        uint32_t vertexIterator = 0;
        uint32_t indexIterator = 0;

        RenderVertex2D * verticesBuffer = m_vertices.empty() == false ? m_vertices.data() : nullptr;
        RenderIndex * indicesBuffer = m_indices.empty() == false ? m_indices.data() : nullptr;

        const mt::mat4f & wm = this->getWorldMatrix();

        const Mengine::RenderInterface * render = this->getRender();

        Color totalColor;
        render->calcTotalColor( &totalColor );

        float totalColorR = totalColor.getR();
        float totalColorG = totalColor.getG();
        float totalColorB = totalColor.getB();
        float totalColorA = totalColor.getA();

        ae_voidptr_t compositionCameraUserdata = ae_get_movie_composition_camera_userdata( m_composition );
        AE_UNUSED( compositionCameraUserdata );

        ae_uint32_t computeMovieMeshIterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &computeMovieMeshIterator, &mesh ) == AE_TRUE )
        {
            RenderContext context;

            context = *_context;

            if( mesh.camera_userdata != nullptr )
            {
                Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(mesh.camera_userdata);

                context.camera = camera->projection.get();
                context.viewport = camera->viewport.get();
            }

            Movie2ScissorPtr scissor;
            if( mesh.viewport != nullptr )
            {
                scissor = Helper::makeFactorableUnique<Movie2Scissor>( MENGINE_DOCUMENT_FORWARD );

                scissor->setScissorViewport( wm, mesh.viewport );

                context.scissor = scissor.get();
            }
            else
            {
                context.scissor = _context->scissor;
            }

            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, _renderPipeline, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, _renderPipeline, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, _renderPipeline, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        Node * node = Helper::reinterpretNodeCast<Node *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, _renderPipeline, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SHAPE_POLYGON:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( totalColorR * mesh.color.r, totalColorG * mesh.color.g, totalColorB * mesh.color.b, totalColorA * mesh.opacity );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            v.color = totalMeshColor;
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        EMaterialBlendMode blendMode = Detail::getMovieBlendMode( mesh.blend_mode );

                        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
                            ->getSolidMaterial( blendMode, false );

                        _renderPipeline->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOLID:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( totalColorR * mesh.color.r, totalColorG * mesh.color.g, totalColorB * mesh.color.b, totalColorA * mesh.opacity );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            v.color = totalMeshColor;
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        EMaterialBlendMode blendMode = Detail::getMovieBlendMode( mesh.blend_mode );

                        if( mesh.shader_userdata == AE_NULLPTR )
                        {
                            const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
                                ->getSolidMaterial( blendMode, false );

                            _renderPipeline->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                        }
                        else
                        {
                            Detail::Movie2ShaderDesc * shaderDesc = reinterpret_cast<Detail::Movie2ShaderDesc *>(mesh.shader_userdata);

                            const RenderMaterialInterfacePtr & material = shaderDesc->materials[blendMode][0];

                            _renderPipeline->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(mesh.resource);

                        ae_userdata_t resourceUserdata = ae_get_movie_resource_userdata( mesh.resource );

                        Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

                        ResourceImage * resourceImage = imageDesc->resourceImage;

                        const Color & imageColor = resourceImage->getColor();

                        float r = totalColorR * mesh.color.r * imageColor.getR();
                        float g = totalColorG * mesh.color.g * imageColor.getG();
                        float b = totalColorB * mesh.color.b * imageColor.getB();
                        float a = totalColorA * mesh.opacity * imageColor.getA();

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( r, g, b, a );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            v.color = totalMeshColor;
                        }

                        if( mesh.uv_cache_userdata == AE_NULLPTR )
                        {
                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                const float * uv = mesh.uv[index];

                                mt::vec2f vuv;
                                vuv.from_f2( uv );

                                resourceImage->correctUV( 0, vuv, v.uv + 0 );
                                resourceImage->correctUV( 1, vuv, v.uv + 1 );
                            }
                        }
                        else
                        {
                            const mt::vec2f * uvs = reinterpret_cast<const mt::vec2f *>(mesh.uv_cache_userdata);

                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                const mt::vec2f & uv = uvs[index];

                                v.uv[0] = uv;
                                v.uv[1] = uv;
                            }
                        }

                        if( mesh.shader_userdata != AE_NULLPTR )
                        {
                            Detail::Movie2ShaderDesc * shaderDesc = reinterpret_cast<Detail::Movie2ShaderDesc *>(mesh.shader_userdata);

                            if( (shaderDesc->flags & AE_MOVIE_EXTENSION_SHADER_FLAG_UVSL) == AE_MOVIE_EXTENSION_SHADER_FLAG_UVSL )
                            {
                                const RenderProgramVariableInterfacePtr & programVariable = shaderDesc->programVariable;

                                mt::vec2f uvZero;
                                mt::vec2f uvOne;
                                resourceImage->correctUV( 0, mt::vec2f( 0.f, 0.f ), &uvZero );
                                resourceImage->correctUV( 0, mt::vec2f( 1.f, 1.f ), &uvOne );

                                const RenderTextureInterfacePtr & texture = resourceImage->getTexture( 0 );

                                const RenderImageInterfacePtr & image = texture->getImage();

                                float textureWidthInv = image->getHWWidthInv();
                                float textureHeightInv = image->getHWHeightInv();

                                float u = uvOne.x - uvZero.x;

                                float ou = uvZero.x - movieResourceImage->offset_x * textureWidthInv;
                                float du = movieResourceImage->base_width / movieResourceImage->trim_width * u;

                                float v = uvOne.y - uvZero.y;

                                float ov = uvZero.y - movieResourceImage->offset_y * textureHeightInv;
                                float dv = movieResourceImage->base_height / movieResourceImage->trim_height * v;

                                float uvsl[4] = {-ou, -ov, 1.f / du, 1.f / dv};
                                programVariable->updatePixelVariables( 0, uvsl, 4, 1 );
                            }
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        EMaterialBlendMode blendMode = Detail::getMovieBlendMode( mesh.blend_mode );

                        if( mesh.shader_userdata == AE_NULLPTR )
                        {
                            RenderMaterialInterfacePtr material = imageDesc->materials[blendMode];

                            _renderPipeline->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                        }
                        else
                        {
                            Detail::Movie2ShaderDesc * shaderDesc = reinterpret_cast<Detail::Movie2ShaderDesc *>(mesh.shader_userdata);

                            ConstString materialName;

                            MENGINE_ASSERTION_FATAL( blendMode == EMB_NORMAL, "movie2 '%s' invalid support shader material blend mode [%u] (Normal)"
                                , this->getName().c_str()
                                , blendMode
                            );

                            RenderMaterialInterfacePtr material = shaderDesc->materials[EMB_NORMAL][resourceImage->getTexture( 1 ) == nullptr ? 0 : 1];

                            const RenderProgramVariableInterfacePtr & programVariable = shaderDesc->programVariable;

                            _renderPipeline->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        Surface * surface = Helper::reinterpretSurfaceCast<Surface *>( mesh.element_userdata );

                        const Color & surfaceColor = surface->getColor();

                        float r = totalColorR * mesh.color.r * surfaceColor.getR();
                        float g = totalColorG * mesh.color.g * surfaceColor.getG();
                        float b = totalColorB * mesh.color.b * surfaceColor.getB();
                        float a = totalColorA * mesh.opacity * surfaceColor.getA();

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( r, g, b, a );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            v.color = totalMeshColor;
                        }

                        if( mesh.uv_cache_userdata == AE_NULLPTR )
                        {
                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                const float * uv = mesh.uv[index];

                                mt::vec2f vuv;
                                vuv.from_f2( uv );

                                surface->correctUV( 0, vuv, v.uv + 0 );
                                surface->correctUV( 1, vuv, v.uv + 1 );
                            }
                        }
                        else
                        {
                            const mt::vec2f * uvs = reinterpret_cast<const mt::vec2f *>(mesh.uv_cache_userdata);

                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                const mt::vec2f & uv = uvs[index];

                                v.uv[0] = uv;
                                v.uv[1] = uv;
                            }
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        const RenderMaterialInterfacePtr & material = surface->getMaterial();

                        _renderPipeline->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                    }break;
                default:
                    break;
                }
            }
            else
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        const SurfaceTrackMatte * surfaceTrackMatte = Helper::reinterpretSurfaceCast<const SurfaceTrackMatte *>( mesh.element_userdata );

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        const ResourceImagePtr & resourceImage = surfaceTrackMatte->getResourceImage();
                        const ResourceImagePtr & resourceTrackMatteImage = surfaceTrackMatte->getResourceTrackMatteImage();

                        const Detail::TrackMatteDesc * trackMatteDesc = reinterpret_cast<const Detail::TrackMatteDesc *>(mesh.track_matte_userdata);

                        const aeMovieRenderMesh * trackMatteMesh = &trackMatteDesc->mesh;

                        const Color & imageColor = resourceImage->getColor();

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( totalColorR * mesh.color.r * imageColor.getR(), totalColorG * mesh.color.g * imageColor.getG(), totalColorB * mesh.color.b * imageColor.getB(), totalColorA * mesh.opacity * imageColor.getA() );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            const float * uv = mesh.uv[index];

                            mt::vec2f vuv;
                            vuv.from_f2( uv );

                            resourceImage->correctUV( 0, vuv, v.uv + 0 );

                            mt::vec2f uvTrackMatte;
                            mt::uv4_calc_point(
                                mt::vec2f( trackMatteMesh->position[0] ),
                                mt::vec2f( trackMatteMesh->position[1] ),
                                mt::vec2f( trackMatteMesh->position[2] ),
                                mt::vec2f( trackMatteMesh->uv[0] ),
                                mt::vec2f( trackMatteMesh->uv[1] ),
                                mt::vec2f( trackMatteMesh->uv[2] ),
                                vertexPosition.to_vec2f(),
                                &uvTrackMatte
                            );

                            resourceTrackMatteImage->correctUV( 1, uvTrackMatte, v.uv + 1 );

                            v.color = totalMeshColor;
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        const RenderProgramVariableInterfacePtr & programVariable = surfaceTrackMatte->getProgramVariable();

                        float uvbb[4];
                        uvbb[0] = StdLimits::numeric_limits<float>::max();
                        uvbb[1] = StdLimits::numeric_limits<float>::max();
                        uvbb[2] = -StdLimits::numeric_limits<float>::max();
                        uvbb[3] = -StdLimits::numeric_limits<float>::max();

                        for( uint32_t index = 0; index != trackMatteMesh->vertexCount; ++index )
                        {
                            const float * uv = trackMatteMesh->uv[index];

                            mt::vec2f vuv;
                            vuv.from_f2( uv );

                            mt::vec2f uvCorrect;
                            resourceTrackMatteImage->correctUV( 1, vuv, &uvCorrect );

                            if( uvbb[0] > uvCorrect.x )
                            {
                                uvbb[0] = uvCorrect.x;
                            }

                            if( uvbb[1] > uvCorrect.y )
                            {
                                uvbb[1] = uvCorrect.y;
                            }

                            if( uvbb[2] < uvCorrect.x )
                            {
                                uvbb[2] = uvCorrect.x;
                            }

                            if( uvbb[3] < uvCorrect.y )
                            {
                                uvbb[3] = uvCorrect.y;
                            }
                        }

                        programVariable->updatePixelVariables( 0, uvbb, 4, 1 );

                        const RenderMaterialInterfacePtr & material = surfaceTrackMatte->getMaterial();

                        _renderPipeline->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        SurfaceTrackMatte * surfaceTrackMatte = Helper::reinterpretSurfaceCast<SurfaceTrackMatte *>( mesh.element_userdata );

                        RenderVertex2D * vertices = verticesBuffer + vertexIterator;
                        vertexIterator += mesh.vertexCount;

                        Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(ae_get_movie_resource_userdata( mesh.resource ));

                        ResourceImage * resourceImage = imageDesc->resourceImage;

                        surfaceTrackMatte->setResourceImage( ResourceImagePtr::from( resourceImage ) );

                        if( surfaceTrackMatte->isCompile() == false )
                        {
                            continue;
                        }

                        const ResourceImagePtr & resourceTrackMatteImage = surfaceTrackMatte->getResourceTrackMatteImage();

                        const Detail::TrackMatteDesc * trackMatteDesc = reinterpret_cast<const Detail::TrackMatteDesc *>(mesh.track_matte_userdata);

                        const aeMovieRenderMesh * trackMatteMesh = &trackMatteDesc->mesh;

                        const Color & imageColor = resourceImage->getColor();

                        float r = totalColorR * mesh.color.r * imageColor.getR();
                        float g = totalColorG * mesh.color.g * imageColor.getG();
                        float b = totalColorB * mesh.color.b * imageColor.getB();
                        float a = totalColorA * mesh.opacity * imageColor.getA();

                        ColorValue_ARGB totalMeshColor = Helper::makeRGBAF( r, g, b, a );

                        if( (totalMeshColor & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            const float * p = mesh.position[index];

                            mt::vec3f vertexPosition;
                            vertexPosition.from_f3( p );

                            mt::mul_v3_v3_m4( &v.position, vertexPosition, wm );

                            const float * uv = mesh.uv[index];

                            mt::vec2f vuv;
                            vuv.from_f2( uv );

                            resourceImage->correctUV( 0, vuv, v.uv + 0 );

                            mt::vec2f uvTrackMatte;
                            mt::uv4_calc_point(
                                mt::vec2f( trackMatteMesh->position[0] ),
                                mt::vec2f( trackMatteMesh->position[1] ),
                                mt::vec2f( trackMatteMesh->position[2] ),
                                mt::vec2f( trackMatteMesh->uv[0] ),
                                mt::vec2f( trackMatteMesh->uv[1] ),
                                mt::vec2f( trackMatteMesh->uv[2] ),
                                vertexPosition.to_vec2f(),
                                &uvTrackMatte
                            );

                            resourceTrackMatteImage->correctUV( 1, uvTrackMatte, v.uv + 1 );

                            v.color = totalMeshColor;
                        }

                        RenderIndex * indices = indicesBuffer + indexIterator;
                        indexIterator += mesh.indexCount;

                        Helper::memoryCopyPod( indices, 0, mesh.indices, 0, mesh.indexCount );

                        const RenderProgramVariableInterfacePtr & programVariable = surfaceTrackMatte->getProgramVariable();

                        float uvbb[4];
                        uvbb[0] = StdLimits::numeric_limits<float>::max();
                        uvbb[1] = StdLimits::numeric_limits<float>::max();
                        uvbb[2] = -StdLimits::numeric_limits<float>::max();
                        uvbb[3] = -StdLimits::numeric_limits<float>::max();

                        for( uint32_t index = 0; index != trackMatteMesh->vertexCount; ++index )
                        {
                            const float * uv = trackMatteMesh->uv[index];

                            mt::vec2f vuv;
                            vuv.from_f2( uv );

                            mt::vec2f uvCorrect;
                            resourceTrackMatteImage->correctUV( 1, vuv, &uvCorrect );

                            if( uvbb[0] > uvCorrect.x )
                            {
                                uvbb[0] = uvCorrect.x;
                            }

                            if( uvbb[1] > uvCorrect.y )
                            {
                                uvbb[1] = uvCorrect.y;
                            }

                            if( uvbb[2] < uvCorrect.x )
                            {
                                uvbb[2] = uvCorrect.x;
                            }

                            if( uvbb[3] < uvCorrect.y )
                            {
                                uvbb[3] = uvCorrect.y;
                            }
                        }

                        programVariable->updatePixelVariables( 0, uvbb, 4, 1 );

                        const RenderMaterialInterfacePtr & material = surfaceTrackMatte->getMaterial();

                        _renderPipeline->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
                    }break;
                default:
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_hierarchyChangeParent( Node * _oldParent, Node * _newParent )
    {
        MENGINE_UNUSED( _oldParent );
        MENGINE_UNUSED( _newParent );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_hierarchyAddChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_hierarchyRemoveChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSurface_( const SurfacePtr & _surface )
    {
        MENGINE_ASSERTION_FATAL( StdAlgorithm::find( m_surfaces.begin(), m_surfaces.end(), _surface ) == m_surfaces.end(), "surface '%s' already attach"
            , _surface->getName().c_str()
        );

        if( _surface->compile() == false )
        {
            LOGGER_ERROR( "movie2 '%s' surface '%s' invalid compile"
                , this->getName().c_str()
                , _surface->getName().c_str()
            );
        }

        m_surfaces.emplace_back( _surface );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeSurface_( const SurfacePtr & _surface )
    {
        VectorSurfaces::iterator itFound = StdAlgorithm::find( m_surfaces.begin(), m_surfaces.end(), _surface );

        MENGINE_ASSERTION_FATAL( itFound != m_surfaces.end(), "surface '%s' not attached"
            , _surface->getName().c_str()
        );

        if( this->isActivate() == true )
        {
            _surface->deactivate();
        }

        _surface->release();

        m_surfaces.erase( itFound );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::activateSurfaces_()
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->activate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::deactivateSurfaces_()
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->deactivate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSprite_( uint32_t _index, const ShapeQuadFixedPtr & _sprite )
    {
        MENGINE_ASSERTION_FATAL( m_sprites.exist( _index ) == false, "movie2 '%s' sprite '%u' already exists"
            , this->getName().c_str()
            , _index
        );

        m_sprites.emplace( _index, _sprite );
    }
    //////////////////////////////////////////////////////////////////////////
    const ShapeQuadFixedPtr & Movie2::getSprite_( uint32_t _index ) const
    {
        const ShapeQuadFixedPtr & sprite = m_sprites.find( _index );

        return sprite;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addParticle_( uint32_t _index, const NodePtr & _particleEmitter )
    {
        MENGINE_ASSERTION_FATAL( m_astralaxEmitters.exist( _index ) == false, "movie2 '%s' particle emitter '%u' already exists"
            , this->getName().c_str()
            , _index
        );

        m_astralaxEmitters.emplace( _index, _particleEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Movie2::getParticle_( uint32_t _index ) const
    {
        const NodePtr & particleEmitter = m_astralaxEmitters.find( _index );

        return particleEmitter;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSlot_( uint32_t _index, const Movie2SlotPtr & _slot )
    {
        MENGINE_ASSERTION_FATAL( m_slots.exist( _index ) == false, "movie2 '%s' slot '%u' already exists"
            , this->getName().c_str()
            , _index
        );

        m_slots.emplace( _index, _slot );
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SlotPtr & Movie2::getSlot_( uint32_t _index ) const
    {
        const Movie2SlotPtr & slot = m_slots.find( _index );

        return slot;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSocket_( uint32_t _index, const HotSpotPolygonPtr & _hotspot )
    {
        MENGINE_ASSERTION_FATAL( m_sockets.exist( _index ) == false, "movie2 '%s' socket '%u' already exists"
            , this->getName().c_str()
            , _index
        );

        m_sockets.emplace( _index, _hotspot );
    }
    //////////////////////////////////////////////////////////////////////////
    const HotSpotPolygonPtr & Movie2::getSocket_( uint32_t _index ) const
    {
        const HotSpotPolygonPtr & hotspot = m_sockets.find( _index );

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addText_( uint32_t _index, const TextFieldPtr & _text )
    {
        MENGINE_ASSERTION_FATAL( m_texts.exist( _index ) == false, "movie2 '%s' text '%u' already exists"
            , this->getName().c_str()
            , _index
        );

        m_texts.emplace( _index, _text );
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFieldPtr & Movie2::getText_( uint32_t _index ) const
    {
        const TextFieldPtr & text = m_texts.find( _index );

        return text;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSubMovieComposition_( const ConstString & _name, const Movie2SubCompositionPtr & _subComposition )
    {
        MENGINE_ASSERTION_FATAL( m_subCompositions.exist( _name ) == false, "movie2 '%s' subcomposition '%s' already exists"
            , this->getName().c_str()
            , _name.c_str()
        );

        m_subCompositions.emplace( _name, _subComposition );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addMatrixProxy_( const MatrixProxyPtr & _matrixProxy )
    {
        m_matrixProxies.emplace_back( _matrixProxy );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::createCompositionLayers_()
    {
        const ConstString & groupName = m_resourceMovie2->getGroupName();

        const ResourceMovie2::CompositionDesc * compositionDesc = m_resourceMovie2->getCompositionDesc( m_compositionName );

#if defined(MENGINE_DEBUG)
        if( compositionDesc == nullptr )
        {
            Stringstream ss;

            m_resourceMovie2->foreachCompositionsDesc( [&ss]( const ConstString & _name, const ResourceMovie2::CompositionDesc & _desc )
            {
                if( _desc.master == false )
                {
                    return;
                }

                ss << _name.c_str() << ", ";
            } );

            LOGGER_ERROR( "movie2 '%s' resource '%s' path '%s' invalid get composition name '%s' in [%s]"
                , this->getName().c_str()
                , this->getResourceMovie2()->getName().c_str()
                , Helper::getContentFullPath( m_resourceMovie2->getContent() ).c_str()
                , m_compositionName.c_str()
                , ss.str().c_str()
            );

            return false;
        }
#endif

        m_duration = AE_TIME_MILLISECOND( compositionDesc->duration );
        m_frameDuration = AE_TIME_MILLISECOND( compositionDesc->frameDuration );
        m_hasBounds = compositionDesc->hasBounds;
        m_bounds = compositionDesc->bounds;

        for( const ResourceMovie2::CompositionLayer & layer : compositionDesc->layers )
        {
            if( layer.type == TextField::getFactorableType() )
            {
                TextFieldPtr node = Helper::generateNodeFactorable<TextField>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'TextField'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );

                RenderInterface * nodeRender = node->getRender();
                nodeRender->setExternalRender( true );

                if( Helper::stringizeExistSymbol( layer.name, '#' ) == false )
                {
                    node->setTextId( layer.name );
                    node->setTextAliasEnvironment( m_aliasEnvironment );
                }
                else
                {
                    Char layerName[512 + 1] = {'\0'};
                    StdString::strcpy_safe( layerName, layer.name.c_str(), 512 );

                    Char * delimAliasEnvironment = StdString::strchr( layerName, '#' );

                    *delimAliasEnvironment = '\0';

                    const Char * extraAliasEnvironment = delimAliasEnvironment + 1;

                    ConstString newTextId = Helper::stringizeString( layerName );
                    ConstString newAliasEnvironment = Helper::stringizeString( extraAliasEnvironment );

                    node->setTextId( newTextId );
                    node->setTextAliasEnvironment( newAliasEnvironment );
                }

                node->setPixelsnap( false );

                this->addText_( layer.index, node );

                MatrixProxyPtr matrixProxy = Helper::generateNodeFactorable<MatrixProxy>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'MatrixProxy'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );

                RenderInterface * matrixProxyRender = matrixProxy->getRender();
                matrixProxyRender->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == Movie2Slot::getFactorableType() )
            {
                Movie2SlotPtr node = Helper::generateNodeFactorable<Movie2Slot>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Movie2Slot'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );

                RenderInterface * nodeRender = node->getRender();
                nodeRender->setExternalRender( true );

                const ConstString & movieName = this->getName();
                node->setMovieName( movieName );
                node->setCompositionName( m_compositionName );

                node->setLayerIndex( layer.index );
                node->setDimension( layer.dimension );
                node->setOptions( layer.options );

                this->addSlot_( layer.index, node );

                MatrixProxyPtr matrixProxy = Helper::generateNodeFactorable<MatrixProxy>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'MatrixProxy'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );

                RenderInterface * matrixProxyRender = matrixProxy->getRender();
                matrixProxyRender->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == HotSpotPolygon::getFactorableType() )
            {
                HotSpotPolygonPtr node = Helper::generateNodeFactorable<HotSpotPolygon>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'HotSpotPolygon'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );

                this->addSocket_( layer.index, node );

                MatrixProxyPtr matrixProxy = Helper::generateNodeFactorable<MatrixProxy>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'MatrixProxy'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );

                RenderInterface * matrixProxyRender = matrixProxy->getRender();
                matrixProxyRender->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) )
            {
                NodePtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( Node::getFactorableType(), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'AstralaxEmitter'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );

                RenderInterface * render = node->getRender();
                render->setExternalRender( true );

                this->addParticle_( layer.index, node );

                MatrixProxyPtr matrixProxy = Helper::generateNodeFactorable<MatrixProxy>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'MatrixProxy'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );

                RenderInterface * matrixProxyRender = matrixProxy->getRender();
                matrixProxyRender->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == ShapeQuadFixed::getFactorableType() )
            {
                ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                    ->getResourceReference( groupName, layer.name );

                MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "name '%s' resource '%s' composition '%s' layer '%s' invalid get resource for image '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                    , layer.name.c_str()
                );

                SurfaceImagePtr surface = Helper::generateSurfaceFactorable<SurfaceImage>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'SurfaceImage'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                surface->setResourceImage( resourceImage );

                ShapeQuadFixedPtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( Node::getFactorableType(), layer.type, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Sprite'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );

                RenderInterface * render = node->getRender();

                render->setExternalRender( true );

                node->setSurface( surface );

                this->addSprite_( layer.index, node );

                MatrixProxyPtr matrixProxy = Helper::generateNodeFactorable<MatrixProxy>( MENGINE_DOCUMENT_MESSAGE( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() ) );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'MatrixProxy'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );

                RenderInterface * matrixProxyRender = matrixProxy->getRender();
                matrixProxyRender->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
        }

        for( const ResourceMovie2::CompositionSubComposition & subcomposition : compositionDesc->subcompositions )
        {
            Movie2SubCompositionPtr subComposition = Helper::generateNodeFactorable<Movie2SubComposition>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( subComposition, "name '%s' resource '%s' composition '%s' invalid create 'Movie2SubComposition'"
                , this->getName().c_str()
                , this->getResourceMovie2()->getName().c_str()
                , this->getCompositionName().c_str()
            );

            subComposition->setMovie( this );

            subComposition->setName( subcomposition.name );
            subComposition->setDuration( subcomposition.duration );
            subComposition->setFrameDuration( subcomposition.frameDuration );

            this->addSubMovieComposition_( subcomposition.name, subComposition );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::disposeCompositionLayers_()
    {
        for( const HashtableSubCompositions::value_type & value : m_subCompositions )
        {
            const Movie2SubCompositionPtr & subComposition = value.element;

            EventationInterface * eventation = subComposition->getEventation();

            eventation->removeEvents();
        }

        m_subCompositions.clear();

        for( const HashtableSlots::value_type & value : m_slots )
        {
            const Movie2SlotPtr & slot = value.element;

            slot->dispose();
        }

        m_slots.clear();

        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & hotspot = value.element;

            hotspot->dispose();
        }

        m_sockets.clear();

        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            text->dispose();
        }

        m_texts.clear();

        for( const HashtableSprites::value_type & value : m_sprites )
        {
            const ShapeQuadFixedPtr & sprite = value.element;

            sprite->dispose();
        }

        m_sprites.clear();

        for( const HashtableAstralaxEmitters::value_type & value : m_astralaxEmitters )
        {
            const NodePtr & astralaxEmitter = value.element;

            astralaxEmitter->dispose();
        }

        m_astralaxEmitters.clear();

        for( const MatrixProxyPtr & proxy : m_matrixProxies )
        {
            proxy->dispose();
        }

        m_matrixProxies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_dispose()
    {
        m_resourceMovie2 = nullptr;

        this->disposeCompositionLayers_();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    ae_bool_t Movie2::movieCompositionNodeProvider_( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _node, ae_voidptr_t _userdata )
    {
        Movie2 * movie2 = static_cast<Movie2 *>(_userdata);

        AnimationInterface * movie2Animation = movie2->getAnimation();

        const aeMovieLayerData * layerData = _callbackData->layer_data;

        ae_uint32_t nodeIndex = _callbackData->index;
        const ae_char_t * layerName = ae_get_movie_layer_data_name( layerData );

        ae_bool_t isTrackMatte = ae_is_movie_layer_data_track_mate( layerData );

        if( isTrackMatte == AE_TRUE )
        {
            *_node = AE_NULLPTR;

            return AE_TRUE;
        }

        aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( layerData );

        switch( layerType )
        {
        case AE_MOVIE_LAYER_TYPE_SPRITE:
            {
                const ShapeQuadFixedPtr & node = movie2->getSprite_( nodeIndex );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Sprite'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layerName
                );

                const SurfacePtr & surface = node->getSurface();

                EMaterialBlendMode blendMode = Detail::getMovieLayerBlendMode( layerData );

                surface->setBlendMode( blendMode );

                *_node = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                const TextFieldPtr & node = movie2->getText_( nodeIndex );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'TextField'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layerName
                );

                Detail::updateMatrixProxy( movie2, node.get(), AE_FALSE, _callbackData->matrix, AE_FALSE, _callbackData->color, _callbackData->opacity );

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'h', 'r' ) ) == AE_TRUE )
                {
                    node->setHorizontAlign( ETFHA_RIGHT );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'h', 'c' ) ) == AE_TRUE )
                {
                    node->setHorizontAlign( ETFHA_CENTER );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'h', 'l' ) ) == AE_TRUE )
                {
                    node->setHorizontAlign( ETFHA_LEFT );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'v', 't' ) ) == AE_TRUE )
                {
                    node->setVerticalAlign( ETFVA_TOP );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'v', 'c' ) ) == AE_TRUE )
                {
                    node->setVerticalAlign( ETFVA_CENTER );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'v', 'b' ) ) == AE_TRUE )
                {
                    node->setVerticalAlign( ETFVA_BOTTOM );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'j', 'u' ) ) == AE_TRUE )
                {
                    node->setJustify( true );
                }

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'a', 's' ) ) == AE_TRUE )
                {
                    node->setWrap( false );
                    node->setAutoScale( true );
                }

                bool box = false;
                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', 'b', 'o', 'x' ) ) == AE_TRUE )
                {
                    box = true;
                }

                ae_aabb_t aabb;
                if( ae_get_movie_layer_data_dimension( layerData, &aabb ) == AE_TRUE )
                {
                    float maxLength = aabb.maximal_x - aabb.minimal_x;

                    node->setMaxLength( maxLength );

                    if( box == true )
                    {
                        float maxHeight = aabb.maximal_y - aabb.minimal_y;

                        node->setMaxHeight( maxHeight );
                    }
                }
                else if( box == true )
                {
                    LOGGER_ERROR( "Movie2 '%s' layer '%s' [%u] set |box| but no dimension data"
                        , movie2->getName().c_str()
                        , ae_get_movie_layer_data_name( layerData )
                        , ae_get_movie_layer_data_index( layerData )
                    );

                    return AE_FALSE;
                }

                *_node = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                const Movie2SlotPtr & node = movie2->getSlot_( nodeIndex );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Movie2Slot'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layerName
                );

                *_node = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOCKET:
            {
                const HotSpotPolygonPtr & node = movie2->getSocket_( nodeIndex );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'HotSpotPolygon'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layerName
                );

                const ae_polygon_t * polygon;
                if( ae_get_movie_layer_data_socket_polygon( _callbackData->layer_data, 0, &polygon ) == AE_FALSE )
                {
                    return false;
                }

                Polygon p;
                for( ae_uint32_t index = 0; index != polygon->point_count; ++index )
                {
                    const ae_vector2_t * v = polygon->points + index;

                    mt::vec2f v2( (*v)[0], (*v)[1] );

                    p.append( v2 );
                }

                node->setPolygon( p );

                *_node = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_PARTICLE:
            {
                const NodePtr & node = movie2->getParticle_( nodeIndex );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Node'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layerName
                );

                UnknownAstralaxEmitterInterface * unknownAstralaxEmitter = node->getUnknown();

                if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', '\0', 't' ) ) == AE_TRUE )
                {
                    unknownAstralaxEmitter->setEmitterPositionRelative( true );
                    unknownAstralaxEmitter->setEmitterCameraRelative( false );
                    unknownAstralaxEmitter->setEmitterTranslateWithParticle( false );
                }
                else
                {
                    unknownAstralaxEmitter->setEmitterPositionRelative( false );
                    unknownAstralaxEmitter->setEmitterCameraRelative( false );
                }

                Resource * resourceParticle = Helper::reinterpretResourceCast<Resource *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer_data ) );

                unknownAstralaxEmitter->setResourceAstralax( ResourcePtr::from( resourceParticle ) );

                unknownAstralaxEmitter->setEmitterPositionProviderOriginOffset( -mt::vec3f( 1024.f, 1024.f, 0.f ) );

                AnimationInterface * animation = node->getAnimation();

                ae_float_t layerStretch = ae_get_movie_layer_data_stretch( _callbackData->layer_data );
                animation->setStretch( layerStretch );

                animation->setRelationAnimation( movie2Animation );
                animation->setRelationIncessantly( _callbackData->incessantly );

                *_node = node.get();

                return AE_TRUE;
            }break;
        default:
            break;
        };

        if( _callbackData->track_matte_layer_data != AE_NULLPTR )
        {
            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    SurfaceTrackMattePtr surfaceTrackMatte = Helper::generateSurfaceFactorable<SurfaceTrackMatte>( MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceTrackMatte, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'SurfaceTrackMatte'"
                        , movie2->getName().c_str()
                        , movie2->getResourceMovie2()->getName().c_str()
                        , movie2->getCompositionName().c_str()
                        , layerName
                    );

                    ConstString cName = Helper::stringizeString( layerName );
                    surfaceTrackMatte->setName( cName );

                    MENGINE_ASSERTION_FATAL( ae_get_movie_layer_data_type( _callbackData->track_matte_layer_data ) == AE_MOVIE_LAYER_TYPE_IMAGE, "Movie2 '%s' image layer '%s' [%u] has track_mate_layer '%s' [%u] not image type"
                        , movie2->getName().c_str()
                        , ae_get_movie_layer_data_name( _callbackData->layer_data )
                        , ae_get_movie_layer_data_index( _callbackData->layer_data )
                        , ae_get_movie_layer_data_name( _callbackData->track_matte_layer_data )
                        , ae_get_movie_layer_data_index( _callbackData->track_matte_layer_data )
                    );

                    Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->layer_data ));
                    ResourceImage * resourceImage = imageDesc->resourceImage;

                    surfaceTrackMatte->setResourceImage( ResourceImagePtr::from( resourceImage ) );

                    Movie2Data::ImageDesc * imageTrackMatteDesc = reinterpret_cast<Movie2Data::ImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->track_matte_layer_data ));
                    ResourceImage * resourceTrackMatteImage = imageTrackMatteDesc->resourceImage;

                    surfaceTrackMatte->setResourceTrackMatteImage( ResourceImagePtr::from( resourceTrackMatteImage ) );

                    ae_track_matte_mode_t track_matte_mode = ae_get_movie_layer_data_track_matte_mode( _callbackData->layer_data );

                    switch( track_matte_mode )
                    {
                    case AE_MOVIE_TRACK_MATTE_ALPHA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA_INVERTED );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_LUMA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_LUMA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_LUMA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_LUMA_INVERTED );
                        }break;
                    default:
                        break;
                    }

                    EMaterialBlendMode blendMode = Detail::getMovieLayerBlendMode( layerData );

                    surfaceTrackMatte->setBlendMode( blendMode );

                    movie2->addSurface_( surfaceTrackMatte );

                    *_node = surfaceTrackMatte.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                {
                    SurfaceTrackMattePtr surfaceTrackMatte = Helper::generateSurfaceFactorable<SurfaceTrackMatte>( MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceTrackMatte, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'SurfaceTrackMatte'"
                        , movie2->getName().c_str()
                        , movie2->getResourceMovie2()->getName().c_str()
                        , movie2->getCompositionName().c_str()
                        , layerName
                    );

                    ConstString cName = Helper::stringizeString( layerName );
                    surfaceTrackMatte->setName( cName );

                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _callbackData->layer_data );
                    const aeMovieResourceSequence * resourceSequence = reinterpret_cast<const aeMovieResourceSequence *>(resource);
                    const aeMovieResourceImage * resourceImage = resourceSequence->images[0];
                    const aeMovieResource * resourceImageBase = reinterpret_cast<const aeMovieResource *>(resourceImage);

                    Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(ae_get_movie_resource_userdata( resourceImageBase ));
                    surfaceTrackMatte->setResourceImage( ResourceImagePtr::from( imageDesc->resourceImage ) );

                    MENGINE_ASSERTION_FATAL( ae_get_movie_layer_data_type( _callbackData->track_matte_layer_data ) == AE_MOVIE_LAYER_TYPE_IMAGE, "Movie2 '%s' sequence layer '%s' [%u] has track_mate_layer '%s' [%u] not image type"
                        , movie2->getName().c_str()
                        , ae_get_movie_layer_data_name( _callbackData->layer_data )
                        , ae_get_movie_layer_data_index( _callbackData->layer_data )
                        , ae_get_movie_layer_data_name( _callbackData->track_matte_layer_data )
                        , ae_get_movie_layer_data_index( _callbackData->track_matte_layer_data )
                    );

                    Movie2Data::ImageDesc * imageTrackMatteDesc = reinterpret_cast<Movie2Data::ImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->track_matte_layer_data ));
                    ResourceImage * resourceTrackMatteImage = imageTrackMatteDesc->resourceImage;

                    surfaceTrackMatte->setResourceTrackMatteImage( ResourceImagePtr::from( resourceTrackMatteImage ) );

                    ae_track_matte_mode_t track_matte_mode = ae_get_movie_layer_data_track_matte_mode( _callbackData->layer_data );

                    switch( track_matte_mode )
                    {
                    case AE_MOVIE_TRACK_MATTE_ALPHA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA_INVERTED );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_LUMA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_LUMA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_LUMA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_LUMA_INVERTED );
                        }break;
                    default:
                        break;
                    }

                    EMaterialBlendMode blendMode = Detail::getMovieLayerBlendMode( layerData );

                    surfaceTrackMatte->setBlendMode( blendMode );

                    movie2->addSurface_( surfaceTrackMatte );

                    *_node = surfaceTrackMatte.get();

                    return AE_TRUE;
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    SurfacePtr surfaceVideo = PROTOTYPE_SERVICE()
                        ->generatePrototype( Surface::getFactorableType(), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceVideo, "name '%s' invalid create 'SurfaceVideo'"
                        , movie2->getName().c_str()
                    );

                    ConstString cName = Helper::stringizeString( layerName );
                    surfaceVideo->setName( cName );

                    Resource * resourceVideo = Helper::reinterpretResourceCast<Resource *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer_data ) );

                    UnknownVideoSurfaceInterface * unknownVideoSurface = surfaceVideo->getUnknown();

                    unknownVideoSurface->setResourceVideo( ResourcePtr::from( resourceVideo ) );

                    EMaterialBlendMode blendMode = Detail::getMovieLayerBlendMode( layerData );

                    AnimationInterface * animation = surfaceVideo->getAnimation();

                    ae_float_t layerStretch = ae_get_movie_layer_data_stretch( _callbackData->layer_data );
                    animation->setStretch( layerStretch );

                    animation->setRelationAnimation( movie2Animation );
                    animation->setRelationIncessantly( _callbackData->incessantly );

                    surfaceVideo->setBlendMode( blendMode );

                    movie2->addSurface_( surfaceVideo );

                    *_node = surfaceVideo.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    SurfaceSoundPtr surfaceSound = Helper::generateSurfaceFactorable<SurfaceSound>( MENGINE_DOCUMENT_FACTORABLE_PTR( movie2 ) );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceSound, "name '%s' invalid create 'SurfaceSound'"
                        , movie2->getName().c_str()
                    );

                    ConstString cName = Helper::stringizeString( layerName );
                    surfaceSound->setName( cName );

                    AnimationInterface * animation = surfaceSound->getAnimation();

                    ae_float_t layerStretch = ae_get_movie_layer_data_stretch( _callbackData->layer_data );
                    animation->setStretch( layerStretch );

                    animation->setRelationAnimation( movie2Animation );
                    animation->setRelationIncessantly( _callbackData->incessantly );

                    surfaceSound->setInterpolateVolume( false );

                    if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'v', 'o' ) ) == AE_TRUE )
                    {
                        surfaceSound->setSoundCategory( ES_SOURCE_CATEGORY_VOICE );
                    }

                    if( ae_has_movie_layer_data_option( layerData, AE_OPTION( '\0', '\0', 'm', 'u' ) ) == AE_TRUE )
                    {
                        surfaceSound->setSoundCategory( ES_SOURCE_CATEGORY_MUSIC );
                    }

                    ResourceSound * resourceSound = Helper::reinterpretResourceCast<ResourceSound *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer_data ) );

                    surfaceSound->setResourceSound( Helper::makeIntrusivePtr( resourceSound ) );

                    movie2->addSurface_( surfaceSound );

                    *_node = surfaceSound.get();

                    return AE_TRUE;
                }break;
            default:
                break;
            }
        }

        *_node = AE_NULLPTR;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    ae_void_t Movie2::movieCompositionNodeDeleter_( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _userdata )
    {
        Movie2 * movie2 = static_cast<Movie2 *>(_userdata);
        AE_UNUSED( movie2 );

        ae_bool_t isTrackMatte = ae_is_movie_layer_data_track_mate( _callbackData->layer_data );

        if( isTrackMatte == AE_TRUE )
        {
            return;
        }

        aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer_data );

        if( _callbackData->track_matte_layer_data != AE_NULLPTR )
        {
            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    //SurfaceTrackMatte * surfaceTrackMatte = (SurfaceTrackMatte *)_callbackData->element_userdata;

                    //movie2->removeSurface_( SurfacePtr::from( surfaceTrackMatte ) );
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
                {
                    Node * node = (Node *)_callbackData->element_userdata;

                    UnknownAstralaxEmitterInterface * unknownAstralaxEmitter = node->getUnknown();

                    unknownAstralaxEmitter->setResourceAstralax( nullptr );
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    //Surface * surfaceVideo = (Surface *)_callbackData->element_userdata;

                    //movie2->removeSurface_( SurfacePtr::from( surfaceVideo ) );
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    //Surface * surfaceSound = (Surface *)_callbackData->element_userdata;

                    //movie2->removeSurface_( SurfacePtr::from( surfaceSound ) );
                }break;
            default:
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::movieMarkerCallback_( const aeMovieMarkerCallbackData * _callbackData, ae_userdata_t _userdata )
    {
        Movie2 * movie = static_cast<Movie2 *>( _userdata );

        MarkerDesc marker;
        marker.name = Helper::stringizeString( _callbackData->name );
        marker.time = AE_TIME_MILLISECOND( _callbackData->time );
        marker.iteration = _callbackData->iteration;
        movie->m_pendingMarkers.emplace_back( marker );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::flushMovieMarkers_()
    {
        uint32_t compositionRevision = m_compositionRevision;
        VectorMarkerDescs markers;
        markers.swap( m_pendingMarkers );

        for( const MarkerDesc & marker : markers )
        {
            if( compositionRevision != m_compositionRevision )
            {
                return;
            }

            EVENTABLE_METHOD( EVENT_MOVIE2_MARKER )
                ->onMovieMarker( marker.name, marker.time, marker.iteration );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
