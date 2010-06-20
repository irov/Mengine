#	include "Consts.h"

namespace Menge
{
	void Consts::initialize( ConstManager * _constManager )
	{
		c_builtin_empty = _constManager->genString("");

		c_Entity = _constManager->genString("Entity");
		c_Arrow = _constManager->genString("Arrow");
		c_Scene = _constManager->genString("Scene");
		c_user = _constManager->genString("user");
		c_WhitePixel = _constManager->genString("WhitePixel");
		c_ResourceImageDefault = _constManager->genString("ResourceImageDefault");
		c_CreateImage = _constManager->genString("CreateImage");
	}
}