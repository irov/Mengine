#	pragma once

#	include "Manager/ConstManager.h"

namespace Menge
{
	class Consts
	{
	public:
		static void initialize( ConstManager * _constManager );

	public:
		static ConstString c_builtin_empty;

		static ConstString c_Entity;
		static ConstString c_Arrow;
		static ConstString c_Scene;
		static ConstString c_user;
		static ConstString c_WhitePixel;
		static ConstString c_ResourceImageDefault;
		static ConstString c_CreateImage;
	};
}