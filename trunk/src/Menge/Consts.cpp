#	include "Consts.h"

#   include "Interface/StringizeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Consts, Menge::Consts, Menge::Consts );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Consts::Consts()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Consts::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Consts::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Consts::initialize()
    {
        c_dir = Helper::stringizeString(m_serviceProvider, "dir");
        c_pak = Helper::stringizeString(m_serviceProvider, "pak");
        c_zip = Helper::stringizeString(m_serviceProvider, "zip");
        c_memory = Helper::stringizeString(m_serviceProvider, "memory");
        c_eng =  Helper::stringizeString(m_serviceProvider, "eng");

        c_Default = Helper::stringizeString(m_serviceProvider, "Default");
        c_Main = Helper::stringizeString(m_serviceProvider, "Main");
        c_Homeless = Helper::stringizeString(m_serviceProvider, "Homeless");
        c_Node = Helper::stringizeString(m_serviceProvider, "Node");
        c_Game = Helper::stringizeString(m_serviceProvider, "Game");
        c_Entity = Helper::stringizeString(m_serviceProvider, "Entity");
        c_Arrow = Helper::stringizeString(m_serviceProvider, "Arrow");
        c_Scene = Helper::stringizeString(m_serviceProvider, "Scene");
        c_Layer2D = Helper::stringizeString(m_serviceProvider, "Layer2D");
        c_HotSpot = Helper::stringizeString(m_serviceProvider, "HotSpot");
        c_HotSpotImage = Helper::stringizeString(m_serviceProvider, "HotSpotImage");
        c_HotSpotShape = Helper::stringizeString(m_serviceProvider, "HotSpotShape");
        c_Point = Helper::stringizeString(m_serviceProvider, "Point");
        c_Window = Helper::stringizeString(m_serviceProvider, "Window");
        c_Image = Helper::stringizeString(m_serviceProvider, "Image");
        c_Video = Helper::stringizeString(m_serviceProvider, "Video");
        c_MovieInternalObject = Helper::stringizeString(m_serviceProvider, "MovieInternalObject");
        c_Sound = Helper::stringizeString(m_serviceProvider, "Sound");
        c_Movie = Helper::stringizeString(m_serviceProvider, "Movie");
        c_SubMovie = Helper::stringizeString(m_serviceProvider, "SubMovie");
        c_user = Helper::stringizeString(m_serviceProvider, "user");
        c_WhitePixel = Helper::stringizeString(m_serviceProvider, "WhitePixel");
        c_Resource = Helper::stringizeString(m_serviceProvider, "Resource");
        c_ResourceSound = Helper::stringizeString(m_serviceProvider, "ResourceSound");
        c_ResourceImageInAtlasCombineRGBAndAlpha = Helper::stringizeString(m_serviceProvider, "ResourceImageInAtlasCombineRGBAndAlpha");
        c_ResourceImageInAtlasDefault = Helper::stringizeString(m_serviceProvider, "ResourceImageInAtlasDefault");
        c_ResourceImageSolid = Helper::stringizeString(m_serviceProvider, "ResourceImageSolid");
        c_ResourceImageDefault = Helper::stringizeString(m_serviceProvider, "ResourceImageDefault");
        c_ResourceImageCombineRGBAndAlpha = Helper::stringizeString(m_serviceProvider, "ResourceImageCombineRGBAndAlpha");
        c_ResourceImageDynamic = Helper::stringizeString(m_serviceProvider, "ResourceImageDynamic");
        c_ResourceHotspotImage = Helper::stringizeString(m_serviceProvider, "ResourceHotspotImage");
        c_ResourceImage = Helper::stringizeString(m_serviceProvider, "ResourceImage");
        c_ResourceAnimation = Helper::stringizeString(m_serviceProvider, "ResourceAnimation");
        c_ResourceInternalObject = Helper::stringizeString(m_serviceProvider, "ResourceInternalObject");
        c_ResourceMovie = Helper::stringizeString(m_serviceProvider, "ResourceMovie");
        c_ResourceHIT = Helper::stringizeString(m_serviceProvider, "ResourceHIT");
            //, c_ResourceSequence = Helper::StringizeString(m_serviceProvider, "ResourceSequence");
        c_ResourceVideo = Helper::stringizeString(m_serviceProvider, "ResourceVideo");
        c_ResourceEmitterContainer = Helper::stringizeString(m_serviceProvider, "ResourceEmitterContainer");
        c_CreateImage = Helper::stringizeString(m_serviceProvider, "CreateImage");
        c_CreateTarget = Helper::stringizeString(m_serviceProvider, "CreateTarget");
        c_CreateTexture = Helper::stringizeString(m_serviceProvider, "CreateTexture");		
        c_Sprite = Helper::stringizeString(m_serviceProvider, "Sprite");
        c_Animation = Helper::stringizeString(m_serviceProvider, "Animation");
        c_Camera2D = Helper::stringizeString(m_serviceProvider, "Camera2D");
        c_Camera3D = Helper::stringizeString(m_serviceProvider, "Camera3D");
        c_TextField = Helper::stringizeString(m_serviceProvider, "TextField");
        c_ConsoleFont = Helper::stringizeString(m_serviceProvider, "ConsoleFont");
        c_SoundEmitter = Helper::stringizeString(m_serviceProvider, "SoundEmitter");
        c_RigidBody2D = Helper::stringizeString(m_serviceProvider, "RigidBody2D");
        c_WorldPhysObject = Helper::stringizeString(m_serviceProvider, "WorldPhysObject");
        c_NullTexture = Helper::stringizeString(m_serviceProvider, "NullTexture");
        c_BlendSprite = Helper::stringizeString(m_serviceProvider, "BlendSprite");
        c_SolidSprite = Helper::stringizeString(m_serviceProvider, "SolidSprite");
        c_OnlyColor = Helper::stringizeString(m_serviceProvider, "OnlyColor");
        c_ExternalAlpha = Helper::stringizeString(m_serviceProvider, "ExternalAlpha");
        c_ExternalAlpha_OnlyColor = Helper::stringizeString(m_serviceProvider, "ExternalAlpha_OnlyColor");
        c_Debug = Helper::stringizeString(m_serviceProvider, "Debug");
        c_ParticleIntensive = Helper::stringizeString(m_serviceProvider, "ParticleIntensive");
        c_ParticleBlend = Helper::stringizeString(m_serviceProvider, "ParticleBlend");
        c_Accumulator = Helper::stringizeString(m_serviceProvider, "Accumulator");
        c_ParticleEmitter = Helper::stringizeString(m_serviceProvider, "ParticleEmitter");
        c_LayerCamera2D = Helper::stringizeString(m_serviceProvider, "LayerCamera2D");
        c_MovieSlot = Helper::stringizeString(m_serviceProvider, "MovieSlot");
        c_MovieSocketImage = Helper::stringizeString(m_serviceProvider, "MovieSocketImage");
        c_MovieSocketShape = Helper::stringizeString(m_serviceProvider, "MovieSocketShape");
        c_MovieText = Helper::stringizeString(m_serviceProvider, "MovieText");
        c_MovieSceneEffect = Helper::stringizeString(m_serviceProvider, "MovieSceneEffect");
        c_MovieNullObject = Helper::stringizeString(m_serviceProvider, "MovieNullObject");
        c_MovieEvent = Helper::stringizeString(m_serviceProvider, "MovieEvent");
        c_BlendingModeAdd = Helper::stringizeString(m_serviceProvider, "BlendingModeAdd");
        c_BlendingModeNormal = Helper::stringizeString(m_serviceProvider, "BlendingModeNormal");

        c_AccountEnumerator = Helper::stringizeString(m_serviceProvider, "AccountEnumerator");
        c_DefaultAccountID = Helper::stringizeString(m_serviceProvider, "DefaultAccountID");
        c_SelectAccountID = Helper::stringizeString(m_serviceProvider, "SelectAccountID");
        c_Account = Helper::stringizeString(m_serviceProvider, "Account");
		c_binMovie = Helper::stringizeString(m_serviceProvider, "binMovie");
		c_aekMovie = Helper::stringizeString(m_serviceProvider, "aekMovie");
		c_binToAekMovie = Helper::stringizeString(m_serviceProvider, "binToAekMovie");
		

        return true;
    }
}