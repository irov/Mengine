#	include "Consts.h"

#   include "Interface/StringizeInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Consts::Consts( ServiceProviderInterface * _serviceProvider )
    {
        c_dir = Helper::stringizeString(_serviceProvider, "dir");
        c_pak = Helper::stringizeString(_serviceProvider, "pak");
        c_zip = Helper::stringizeString(_serviceProvider, "zip");
        c_memory = Helper::stringizeString(_serviceProvider, "memory");
        c_eng =  Helper::stringizeString(_serviceProvider, "eng");

        c_Default = Helper::stringizeString(_serviceProvider, "Default");
        c_Main = Helper::stringizeString(_serviceProvider, "Main");
        c_Homeless = Helper::stringizeString(_serviceProvider, "Homeless");
        c_Node = Helper::stringizeString(_serviceProvider, "Node");
        c_Game = Helper::stringizeString(_serviceProvider, "Game");
        c_Entity = Helper::stringizeString(_serviceProvider, "Entity");
        c_Arrow = Helper::stringizeString(_serviceProvider, "Arrow");
        c_Scene = Helper::stringizeString(_serviceProvider, "Scene");
        c_Layer2D = Helper::stringizeString(_serviceProvider, "Layer2D");
        c_HotSpot = Helper::stringizeString(_serviceProvider, "HotSpot");
        c_HotSpotImage = Helper::stringizeString(_serviceProvider, "HotSpotImage");
        c_Point = Helper::stringizeString(_serviceProvider, "Point");
        c_Window = Helper::stringizeString(_serviceProvider, "Window");
        c_Image = Helper::stringizeString(_serviceProvider, "Image");
        c_Video = Helper::stringizeString(_serviceProvider, "Video");
        c_MovieInternalObject = Helper::stringizeString(_serviceProvider, "MovieInternalObject");
        c_Sound = Helper::stringizeString(_serviceProvider, "Sound");
        c_Movie = Helper::stringizeString(_serviceProvider, "Movie");
        c_SubMovie = Helper::stringizeString(_serviceProvider, "SubMovie");
        c_user = Helper::stringizeString(_serviceProvider, "user");
        c_WhitePixel = Helper::stringizeString(_serviceProvider, "WhitePixel");
        c_ResourceSound = Helper::stringizeString(_serviceProvider, "ResourceSound");
        c_ResourceImageInAtlasCombineRGBAndAlpha = Helper::stringizeString(_serviceProvider, "ResourceImageInAtlasCombineRGBAndAlpha");
        c_ResourceImageInAtlasDefault = Helper::stringizeString(_serviceProvider, "ResourceImageInAtlasDefault");
        c_ResourceImageSolid = Helper::stringizeString(_serviceProvider, "ResourceImageSolid");
        c_ResourceImageDefault = Helper::stringizeString(_serviceProvider, "ResourceImageDefault");
        c_ResourceImageCombineRGBAndAlpha = Helper::stringizeString(_serviceProvider, "ResourceImageCombineRGBAndAlpha");
        c_ResourceImageDynamic = Helper::stringizeString(_serviceProvider, "ResourceImageDynamic");
        c_ResourceHotspotImage = Helper::stringizeString(_serviceProvider, "ResourceHotspotImage");
        c_ResourceImage = Helper::stringizeString(_serviceProvider, "ResourceImage");
        c_ResourceAnimation = Helper::stringizeString(_serviceProvider, "ResourceAnimation");
        c_ResourceInternalObject = Helper::stringizeString(_serviceProvider, "ResourceInternalObject");
        c_ResourceMovie = Helper::stringizeString(_serviceProvider, "ResourceMovie");
        c_ResourceHIT = Helper::stringizeString(_serviceProvider, "ResourceHIT");
            //, c_ResourceSequence = Helper::StringizeString(_serviceProvider, "ResourceSequence");
        c_ResourceVideo = Helper::stringizeString(_serviceProvider, "ResourceVideo");
        c_ResourceEmitterContainer = Helper::stringizeString(_serviceProvider, "ResourceEmitterContainer");
        c_CreateImage = Helper::stringizeString(_serviceProvider, "CreateImage");
        c_CreateTarget = Helper::stringizeString(_serviceProvider, "CreateTarget");
        c_CreateTexture = Helper::stringizeString(_serviceProvider, "CreateTexture");		
        c_Sprite = Helper::stringizeString(_serviceProvider, "Sprite");
        c_Animation = Helper::stringizeString(_serviceProvider, "Animation");
        c_Camera2D = Helper::stringizeString(_serviceProvider, "Camera2D");
        c_Camera3D = Helper::stringizeString(_serviceProvider, "Camera3D");
        c_TextField = Helper::stringizeString(_serviceProvider, "TextField");
        c_ConsoleFont = Helper::stringizeString(_serviceProvider, "ConsoleFont");
        c_SoundEmitter = Helper::stringizeString(_serviceProvider, "SoundEmitter");
        c_RigidBody2D = Helper::stringizeString(_serviceProvider, "RigidBody2D");
        c_WorldPhysObject = Helper::stringizeString(_serviceProvider, "WorldPhysObject");
        c_NullTexture = Helper::stringizeString(_serviceProvider, "NullTexture");
        c_BlendSprite = Helper::stringizeString(_serviceProvider, "BlendSprite");
        c_SolidSprite = Helper::stringizeString(_serviceProvider, "SolidSprite");
        c_OnlyColor = Helper::stringizeString(_serviceProvider, "OnlyColor");
        c_ExternalAlpha = Helper::stringizeString(_serviceProvider, "ExternalAlpha");
        c_Debug = Helper::stringizeString(_serviceProvider, "Debug");
        c_ParticleIntensive = Helper::stringizeString(_serviceProvider, "ParticleIntensive");
        c_ParticleBlend = Helper::stringizeString(_serviceProvider, "ParticleBlend");
        c_Accumulator = Helper::stringizeString(_serviceProvider, "Accumulator");
        c_ParticleEmitter = Helper::stringizeString(_serviceProvider, "ParticleEmitter");
        c_LayerCamera2D = Helper::stringizeString(_serviceProvider, "LayerCamera2D");
        c_MovieSlot = Helper::stringizeString(_serviceProvider, "MovieSlot");
        c_MovieSocket = Helper::stringizeString(_serviceProvider, "MovieSocket");
        c_MovieText = Helper::stringizeString(_serviceProvider, "MovieText");
        c_MovieSceneEffect = Helper::stringizeString(_serviceProvider, "MovieSceneEffect");
        c_MovieNullObject = Helper::stringizeString(_serviceProvider, "MovieNullObject");
        c_MovieEvent = Helper::stringizeString(_serviceProvider, "MovieEvent");
        c_BlendingModeAdd = Helper::stringizeString(_serviceProvider, "BlendingModeAdd");
        c_BlendingModeNormal = Helper::stringizeString(_serviceProvider, "BlendingModeNormal");
    }
    //////////////////////////////////////////////////////////////////////////
    const Consts & getConsts( ServiceProviderInterface * _serviceProvider )
    {
        static Consts s_consts(_serviceProvider);

        return s_consts;
    }
}