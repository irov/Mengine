#	include "Consts.h"

namespace Menge
{
	void Consts::initialize( ConstManager * _constManager )
	{
		c_builtin_empty = _constManager->genString("");

		c_pak = _constManager->genString("pak");
		c_zip = _constManager->genString("zip");
		c_memory = _constManager->genString("memory");

		c_Default = _constManager->genString("Default");
		c_Game = _constManager->genString("Game");
		c_Entity = _constManager->genString("Entity");
		c_Arrow = _constManager->genString("Arrow");
		c_Scene = _constManager->genString("Scene");
		c_Window = _constManager->genString("Window");
		c_Image = _constManager->genString("Image");
		c_Video = _constManager->genString("Video");
		c_Sound = _constManager->genString("Sound");
		c_user = _constManager->genString("user");
		c_WhitePixel = _constManager->genString("WhitePixel");
		c_ResourceImageDefault = _constManager->genString("ResourceImageDefault");
		c_ResourceImageDynamic = _constManager->genString("ResourceImageDynamic");
		c_CreateImage = _constManager->genString("CreateImage");
		c_CreateTarget = _constManager->genString("CreateTarget");
		c_CreateTexture = _constManager->genString("CreateTexture");		
		c_Sprite = _constManager->genString("Sprite");
		c_Camera2D = _constManager->genString("Camera2D");
		c_TextField = _constManager->genString("TextField");
		c_ConsoleFont = _constManager->genString("ConsoleFont");
		c_SoundEmitter = _constManager->genString("SoundEmitter");
		c_RigidBody2D = _constManager->genString("RigidBody2D");
		c_WorldPhysObject = _constManager->genString("WorldPhysObject");
		c_NullTexture = _constManager->genString("NullTexture");
	}
}