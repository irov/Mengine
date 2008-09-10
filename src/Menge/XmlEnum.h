#	pragma once

namespace Menge
{
	enum XMLDefine
	{
		DEF_BEGIN,
		DEF_Layer2D,
		DEF_Sprite,
		DEF_Animation,
		DEF_TextField,
		DEF_HotSpot,
		DEF_Point,
		DEF_LAST
	};

	enum XMLAttribute
	{
		ATTR_BEGIN,
		ATTR_Name,
		ATTR_Type,
		ATTR_Value,
		ATTR_Position,
		ATTR_Direction,
		ATTR_Path,
		ATTR_LAST
	};

	enum XMLType
	{
		TYPE_BEGIN,
		TYPE_Define,
		TYPE_Int,
		TYPE_Bool,
		TYPE_Float,
		TYPE_Float2,
		TYPE_Float4,
		TYPE_Mat3,
		TYPE_String,
		TYPE_LAST
	};

	enum XMLTag
	{
		TAG_BEGIN,
		TAG_VALUE,

		TAG_Scene,

		TAG_Node,
		TAG_Size,
		TAG_Scale,
		TAG_Point,
		TAG_Main,
		TAG_ImageMap,
		TAG_Transformation,
		TAG_RenderArea,
		TAG_Animation,
		TAG_Looping,
		TAG_AutoStart,
		TAG_Font,
		TAG_Color,
		TAG_CenterAlign,
		TAG_OutlineColor,
		TAG_Outline,
		TAG_CharOffset,

		TAG_DataBlock,
		TAG_File,
		TAG_Sequences,
		TAG_SequenceArray,

		TAG_Resources,

		TAG_Scenes,

		TAG_LAST
	};
}