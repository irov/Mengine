#	pragma once

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class Consts
		: public Holder<Consts>
	{
	public:
		Consts();

	public:
		ConstString c_builtin_empty;

		ConstString c_pak;
		ConstString c_zip;
		ConstString c_memory;
		ConstString c_xml2bin;

		ConstString c_Homeless;
		ConstString c_Node;
		ConstString c_Game;
		ConstString c_Default;
		ConstString c_Entity;
		ConstString c_Arrow;
		ConstString c_Scene;
		ConstString c_Window;
		ConstString c_Image;
		ConstString c_Video;
		ConstString c_Sound;
		ConstString c_user;
		ConstString c_WhitePixel;
		ConstString c_ResourceSound;
		ConstString c_ResourceImageDefault;
		ConstString c_ResourceImageDynamic;
		ConstString c_ResourceHotspotImage;
		ConstString c_ResourceAnimation;
		ConstString c_CreateImage;
		ConstString c_CreateTarget;
		ConstString c_CreateTexture;
		ConstString c_Sprite;
		ConstString c_Camera2D;
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

		ConstString c_LayerCamera2D;
	};
}

#	define CONST_STRING( S )\
	(Consts::get()->c_##S)