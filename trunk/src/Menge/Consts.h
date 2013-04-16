#	pragma once

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	class Consts
	{
	public:
		Consts( ServiceProviderInterface * _serviceProvider );

	public:
		ConstString c_dir;
		ConstString c_pak;
		ConstString c_zip;
		ConstString c_memory;
        ConstString c_eng;
		
        ConstString c_Main;
		ConstString c_Homeless;
		ConstString c_Node;
		ConstString c_Game;
		ConstString c_Default;
		ConstString c_Entity;
		ConstString c_Arrow;
		ConstString c_Scene;
        ConstString c_Layer2D;
        ConstString c_HotSpot;
        ConstString c_HotSpotImage;
        ConstString c_Point;
		ConstString c_Window;
		ConstString c_Image;
		ConstString c_Video;
		ConstString c_MovieInternalObject;
		ConstString c_Sound;
		ConstString c_Movie;
        ConstString c_SubMovie;
		ConstString c_user;
		ConstString c_WhitePixel;
		ConstString c_ResourceSound;
		ConstString c_ResourceImageInAtlasDefault;
		ConstString c_ResourceImageInAtlasCombineRGBAndAlpha;
		ConstString c_ResourceImageSolid;
		ConstString c_ResourceImageDefault;
		ConstString c_ResourceImageCombineRGBAndAlpha;
		ConstString c_ResourceImageDynamic;
		ConstString c_ResourceHotspotImage;
		ConstString c_ResourceImage;
		ConstString c_ResourceAnimation;
		ConstString c_ResourceInternalObject;
		ConstString c_ResourceMovie;
        ConstString c_ResourceHIT;
		//ConstString c_ResourceSequence;
		ConstString c_ResourceVideo;
		ConstString c_ResourceEmitterContainer;
		ConstString c_CreateImage;
		ConstString c_CreateTarget;
		ConstString c_CreateTexture;
		ConstString c_Sprite;
		ConstString c_Animation;
		ConstString c_Camera2D;
		ConstString c_Camera3D;
		ConstString c_TextField;
		ConstString c_ConsoleFont;		
		ConstString c_SoundEmitter;
		ConstString c_RigidBody2D;
		ConstString c_WorldPhysObject;
		ConstString c_NullTexture;
		ConstString c_BlendSprite;
		ConstString c_SolidSprite;
		ConstString c_OnlyColor;
		ConstString c_ExternalAlpha;
		ConstString c_Debug;
		ConstString c_ParticleIntensive;
		ConstString c_ParticleBlend;
		ConstString c_Accumulator;
		ConstString c_ParticleEmitter;

		ConstString c_LayerCamera2D;

		ConstString c_MovieSlot;
        ConstString c_MovieSocket;
        ConstString c_MovieSceneEffect;
        ConstString c_MovieText;
		ConstString c_MovieNullObject;
        ConstString c_MovieEvent;

		ConstString c_BlendingModeAdd;
		ConstString c_BlendingModeNormal;
	};

    const Consts & getConsts( ServiceProviderInterface * _serviceProvider );
}

#	define CONST_STRING( ServiceProvider, String )\
	(getConsts(ServiceProvider).c_##String)

