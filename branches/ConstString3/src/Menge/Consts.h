#	pragma once

#	include "ConstManager.h"

namespace Menge
{
	class Consts
	{
	public:
		static void initialize( ConstManager * _constManager );

	public:
		static ConstString c_builtin_empty;

		static ConstString c_pak;
		static ConstString c_zip;
		static ConstString c_memory;

		static ConstString c_Game;
		static ConstString c_Default;
		static ConstString c_Entity;
		static ConstString c_Arrow;
		static ConstString c_Scene;
		static ConstString c_Window;
		static ConstString c_Image;
		static ConstString c_Video;
		static ConstString c_Sound;
		static ConstString c_user;
		static ConstString c_WhitePixel;
		static ConstString c_ResourceImageDefault;
		static ConstString c_ResourceImageDynamic;
		static ConstString c_CreateImage;
		static ConstString c_CreateTarget;
		static ConstString c_CreateTexture;
		static ConstString c_Sprite;
		static ConstString c_Camera2D;
		static ConstString c_TextField;
		static ConstString c_ConsoleFont;		
		static ConstString c_SoundEmitter;
		static ConstString c_RigidBody2D;
		static ConstString c_WorldPhysObject;
		static ConstString c_NullTexture;
	};
}