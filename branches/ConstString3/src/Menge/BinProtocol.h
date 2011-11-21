#	pragma once

namespace Menge
{
	namespace Protocol
	{
		static const int version = 1321898514;
		struct AccountID
		{
			static const int id = 117;
		};
		struct AccountID_Value
		{
			static const int id = 118;
			typedef Menge::String Type;
		};
		struct AllowFullscreenSwitchShortcut
		{
			static const int id = 11;
		};
		struct AllowFullscreenSwitchShortcut_Value
		{
			static const int id = 12;
			typedef bool Type;
		};
		struct AllowSleep
		{
			static const int id = 186;
		};
		struct AllowSleep_Value
		{
			static const int id = 187;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 516;
		};
		struct Alpha_Value
		{
			static const int id = 517;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 154;
		};
		struct AlphaTest_Value
		{
			static const int id = 155;
			typedef float Type;
		};
		struct AlreadyRunningPolicy
		{
			static const int id = 9;
		};
		struct AlreadyRunningPolicy_Value
		{
			static const int id = 10;
			typedef int Type;
		};
		struct Amplitude
		{
			static const int id = 156;
		};
		struct Amplitude_Value
		{
			static const int id = 157;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 200;
		};
		struct Angle_Value
		{
			static const int id = 201;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 184;
		};
		struct AngularDamping_Value
		{
			static const int id = 185;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 137;
		};
		struct Animation_Name
		{
			static const int id = 138;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh
		{
			static const int id = 329;
		};
		struct AnimationMesh_In
		{
			static const int id = 328;
			typedef float Type;
		};
		struct AnimationMesh_Name
		{
			static const int id = 324;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh_Out
		{
			static const int id = 330;
			typedef float Type;
		};
		struct AnimationMesh_Texture
		{
			static const int id = 326;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 482;
		};
		struct Arrow
		{
			static const int id = 488;
		};
		struct Arrow_Name
		{
			static const int id = 489;
			typedef Menge::ConstString Type;
		};
		struct Arrows
		{
			static const int id = 69;
		};
		struct Arrows_Path
		{
			static const int id = 70;
			typedef Menge::String Type;
		};
		struct AutoPlay
		{
			static const int id = 162;
		};
		struct AutoPlay_Value
		{
			static const int id = 163;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 141;
		};
		struct AutoStart_Value
		{
			static const int id = 142;
			typedef bool Type;
		};
		struct Background
		{
			static const int id = 383;
		};
		struct Background_Codec
		{
			static const int id = 384;
			typedef Menge::ConstString Type;
		};
		struct Background_Image
		{
			static const int id = 382;
			typedef Menge::ConstString Type;
		};
		struct BaseDir
		{
			static const int id = 1;
		};
		struct BaseDir_Value
		{
			static const int id = 2;
			typedef Menge::String Type;
		};
		struct BinaryNode
		{
			static const int id = 123;
		};
		struct BinaryNode_Type
		{
			static const int id = 124;
			typedef Menge::ConstString Type;
		};
		struct BlendDest
		{
			static const int id = 214;
		};
		struct BlendDest_Value
		{
			static const int id = 215;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 212;
		};
		struct BlendSource_Value
		{
			static const int id = 213;
			typedef int Type;
		};
		struct Bottom
		{
			static const int id = 407;
		};
		struct Bottom_Codec
		{
			static const int id = 408;
			typedef Menge::ConstString Type;
		};
		struct Bottom_Image
		{
			static const int id = 406;
			typedef Menge::ConstString Type;
		};
		struct CategoryBits
		{
			static const int id = 178;
		};
		struct CategoryBits_Value
		{
			static const int id = 179;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 475;
		};
		struct Cell_X
		{
			static const int id = 474;
			typedef size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 476;
			typedef size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 208;
		};
		struct CenterAlign_Value
		{
			static const int id = 209;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 281;
		};
		struct Char_code
		{
			static const int id = 278;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 282;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 280;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 276;
			typedef float Type;
		};
		struct CharOffset
		{
			static const int id = 226;
		};
		struct CharOffset_Value
		{
			static const int id = 227;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 145;
		};
		struct ClickOffset_Value
		{
			static const int id = 146;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 252;
		};
		struct ClientSize_Value
		{
			static const int id = 253;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 254;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 255;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 176;
		};
		struct CollisionMask_Value
		{
			static const int id = 177;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 135;
		};
		struct Color_Value
		{
			static const int id = 136;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 344;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 345;
			typedef Menge::String Type;
		};
		struct ContentResolution
		{
			static const int id = 19;
		};
		struct ContentResolution_Value
		{
			static const int id = 20;
			typedef Menge::Resolution Type;
		};
		struct ConvexGeometryFile
		{
			static const int id = 346;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 347;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 417;
		};
		struct Default
		{
			static const int id = 484;
		};
		struct DefaultAccountID
		{
			static const int id = 119;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 120;
			typedef Menge::String Type;
		};
		struct DefaultArrow
		{
			static const int id = 29;
		};
		struct DefaultArrow_Name
		{
			static const int id = 28;
			typedef Menge::ConstString Type;
		};
		struct DefaultArrow_Prototype
		{
			static const int id = 30;
			typedef Menge::ConstString Type;
		};
		struct DefaultVolume
		{
			static const int id = 582;
		};
		struct DefaultVolume_Value
		{
			static const int id = 583;
			typedef float Type;
		};
		struct Density
		{
			static const int id = 170;
		};
		struct Density_Value
		{
			static const int id = 171;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 342;
		};
		struct Depth_Value
		{
			static const int id = 343;
			typedef float Type;
		};
		struct Duration
		{
			static const int id = 584;
		};
		struct Duration_Value
		{
			static const int id = 585;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 240;
		};
		struct Edge_Layer
		{
			static const int id = 241;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 160;
		};
		struct Emitter_Name
		{
			static const int id = 161;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 168;
		};
		struct EmitterRelative_Value
		{
			static const int id = 169;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 131;
		};
		struct Enable_Value
		{
			static const int id = 132;
			typedef bool Type;
		};
		struct Entities
		{
			static const int id = 63;
		};
		struct Entities_Path
		{
			static const int id = 64;
			typedef Menge::String Type;
		};
		struct Entity
		{
			static const int id = 494;
		};
		struct Entity_Name
		{
			static const int id = 491;
			typedef Menge::ConstString Type;
		};
		struct Entity_Prototype
		{
			static const int id = 493;
			typedef Menge::ConstString Type;
		};
		struct Entity_Tag
		{
			static const int id = 495;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 230;
		};
		struct FieldSize_Value
		{
			static const int id = 231;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 315;
		};
		struct File_Alpha
		{
			static const int id = 306;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 296;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 308;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 302;
			typedef mt::vec2f Type;
		};
		struct File_Name
		{
			static const int id = 292;
			typedef Menge::ConstString Type;
		};
		struct File_Offset
		{
			static const int id = 300;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 294;
			typedef Menge::ConstString Type;
		};
		struct File_Size
		{
			static const int id = 304;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 312;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 310;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 298;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 314;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 316;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 289;
		};
		struct Filter_Value
		{
			static const int id = 290;
			typedef size_t Type;
		};
		struct FixedContentResolution
		{
			static const int id = 21;
		};
		struct FixedContentResolution_Value
		{
			static const int id = 22;
			typedef bool Type;
		};
		struct FixedRotation
		{
			static const int id = 190;
		};
		struct FixedRotation_Value
		{
			static const int id = 191;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 265;
		};
		struct Folder_Path
		{
			static const int id = 266;
			typedef Menge::ConstString Type;
		};
		struct Font
		{
			static const int id = 273;
		};
		struct Font_Name
		{
			static const int id = 217;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 274;
			typedef float Type;
		};
		struct Frame
		{
			static const int id = 471;
		};
		struct Frame_UV
		{
			static const int id = 472;
			typedef mt::vec4f Type;
		};
		struct Frequency
		{
			static const int id = 158;
		};
		struct Frequency_Value
		{
			static const int id = 159;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 172;
		};
		struct Friction_Value
		{
			static const int id = 173;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 483;
		};
		struct GamePack
		{
			static const int id = 7;
		};
		struct GamePack_Description
		{
			static const int id = 8;
			typedef Menge::String Type;
		};
		struct GamePack_Name
		{
			static const int id = 4;
			typedef Menge::ConstString Type;
		};
		struct GamePack_Path
		{
			static const int id = 6;
			typedef Menge::String Type;
		};
		struct Glyph
		{
			static const int id = 477;
		};
		struct Glyph_Resource
		{
			static const int id = 478;
			typedef Menge::ConstString Type;
		};
		struct Gravity2D
		{
			static const int id = 443;
		};
		struct Gravity2D_Value
		{
			static const int id = 444;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 432;
		};
		struct GridSize_Value
		{
			static const int id = 433;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 180;
		};
		struct GroupIndex_Value
		{
			static const int id = 181;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 204;
		};
		struct HeadMode_Value
		{
			static const int id = 205;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 198;
		};
		struct Height_Value
		{
			static const int id = 199;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 133;
		};
		struct Hide_Value
		{
			static const int id = 134;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 234;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 235;
			typedef int Type;
		};
		struct Icon
		{
			static const int id = 25;
		};
		struct Icon_Path
		{
			static const int id = 26;
			typedef Menge::String Type;
		};
		struct Image
		{
			static const int id = 269;
		};
		struct Image_Path
		{
			static const int id = 270;
			typedef Menge::ConstString Type;
		};
		struct ImageAlpha
		{
			static const int id = 206;
		};
		struct ImageAlpha_Name
		{
			static const int id = 207;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 152;
		};
		struct ImageIndex_Value
		{
			static const int id = 153;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 150;
		};
		struct ImageMap_Name
		{
			static const int id = 151;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 287;
		};
		struct ImageResource_Name
		{
			static const int id = 288;
			typedef Menge::ConstString Type;
		};
		struct Indices
		{
			static const int id = 331;
		};
		struct Indices_Value
		{
			static const int id = 332;
			typedef std::vector<uint16> Type;
		};
		struct Internal
		{
			static const int id = 590;
		};
		struct Internal_Group
		{
			static const int id = 591;
			typedef Menge::ConstString Type;
		};
		struct Internal_Name
		{
			static const int id = 589;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 188;
		};
		struct IsBullet_Value
		{
			static const int id = 189;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 192;
		};
		struct IsSensor_Value
		{
			static const int id = 193;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 359;
		};
		struct IsStreamable_Value
		{
			static const int id = 360;
			typedef bool Type;
		};
		struct Kerning
		{
			static const int id = 285;
		};
		struct Kerning_advance
		{
			static const int id = 284;
			typedef float Type;
		};
		struct Kerning_id
		{
			static const int id = 286;
			typedef Menge::String Type;
		};
		struct KeyFrame2D
		{
			static const int id = 544;
		};
		struct KeyFrame2D_AnchorPoint
		{
			static const int id = 537;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Opacity
		{
			static const int id = 545;
			typedef float Type;
		};
		struct KeyFrame2D_Position
		{
			static const int id = 539;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Rotation
		{
			static const int id = 543;
			typedef float Type;
		};
		struct KeyFrame2D_Scale
		{
			static const int id = 541;
			typedef mt::vec2f Type;
		};
		struct KeyFrame3D
		{
			static const int id = 580;
		};
		struct KeyFrame3D_AnchorPoint
		{
			static const int id = 573;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Opacity
		{
			static const int id = 581;
			typedef float Type;
		};
		struct KeyFrame3D_Position
		{
			static const int id = 575;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Rotation
		{
			static const int id = 579;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Scale
		{
			static const int id = 577;
			typedef mt::vec2f Type;
		};
		struct LanguagePack
		{
			static const int id = 57;
		};
		struct LanguagePack_Description
		{
			static const int id = 56;
			typedef Menge::String Type;
		};
		struct LanguagePack_Name
		{
			static const int id = 52;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Path
		{
			static const int id = 54;
			typedef Menge::String Type;
		};
		struct LanguagePack_PreLoad
		{
			static const int id = 58;
			typedef bool Type;
		};
		struct Left
		{
			static const int id = 415;
		};
		struct Left_Codec
		{
			static const int id = 416;
			typedef Menge::ConstString Type;
		};
		struct Left_Image
		{
			static const int id = 414;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom
		{
			static const int id = 411;
		};
		struct LeftBottom_Codec
		{
			static const int id = 412;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom_Image
		{
			static const int id = 410;
			typedef Menge::ConstString Type;
		};
		struct LeftTop
		{
			static const int id = 387;
		};
		struct LeftTop_Codec
		{
			static const int id = 388;
			typedef Menge::ConstString Type;
		};
		struct LeftTop_Image
		{
			static const int id = 386;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 228;
		};
		struct LineOffset_Value
		{
			static const int id = 229;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 182;
		};
		struct LinearDamping_Value
		{
			static const int id = 183;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 348;
		};
		struct Loop_Value
		{
			static const int id = 349;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 164;
		};
		struct Looped_Value
		{
			static const int id = 165;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 139;
		};
		struct Looping_Value
		{
			static const int id = 140;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 422;
		};
		struct Main_Value
		{
			static const int id = 423;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 224;
		};
		struct MaxWidth_Value
		{
			static const int id = 225;
			typedef float Type;
		};
		struct Model
		{
			static const int id = 321;
		};
		struct Model_Name
		{
			static const int id = 322;
			typedef Menge::ConstString Type;
		};
		struct Movie
		{
			static const int id = 520;
		};
		struct Movie_Name
		{
			static const int id = 521;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D
		{
			static const int id = 534;
		};
		struct MovieLayer2D_In
		{
			static const int id = 533;
			typedef float Type;
		};
		struct MovieLayer2D_Index
		{
			static const int id = 529;
			typedef size_t Type;
		};
		struct MovieLayer2D_Internal
		{
			static const int id = 531;
			typedef bool Type;
		};
		struct MovieLayer2D_Name
		{
			static const int id = 523;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_Out
		{
			static const int id = 535;
			typedef float Type;
		};
		struct MovieLayer2D_Parent
		{
			static const int id = 525;
			typedef size_t Type;
		};
		struct MovieLayer2D_Source
		{
			static const int id = 527;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D
		{
			static const int id = 570;
		};
		struct MovieLayer3D_CameraAspect
		{
			static const int id = 567;
			typedef float Type;
		};
		struct MovieLayer3D_CameraFOV
		{
			static const int id = 565;
			typedef float Type;
		};
		struct MovieLayer3D_CameraInterest
		{
			static const int id = 563;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_CameraPosition
		{
			static const int id = 561;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_Height
		{
			static const int id = 571;
			typedef float Type;
		};
		struct MovieLayer3D_In
		{
			static const int id = 557;
			typedef float Type;
		};
		struct MovieLayer3D_Index
		{
			static const int id = 553;
			typedef size_t Type;
		};
		struct MovieLayer3D_Internal
		{
			static const int id = 555;
			typedef bool Type;
		};
		struct MovieLayer3D_Name
		{
			static const int id = 547;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Out
		{
			static const int id = 559;
			typedef float Type;
		};
		struct MovieLayer3D_Parent
		{
			static const int id = 549;
			typedef size_t Type;
		};
		struct MovieLayer3D_Source
		{
			static const int id = 551;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Width
		{
			static const int id = 569;
			typedef float Type;
		};
		struct Node
		{
			static const int id = 129;
		};
		struct Node_Name
		{
			static const int id = 126;
			typedef Menge::ConstString Type;
		};
		struct Node_Tag
		{
			static const int id = 130;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 128;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 222;
		};
		struct Outline_Value
		{
			static const int id = 223;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 220;
		};
		struct OutlineColor_Value
		{
			static const int id = 221;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 271;
		};
		struct OutlineImage_Path
		{
			static const int id = 272;
			typedef Menge::ConstString Type;
		};
		struct Pak
		{
			static const int id = 485;
		};
		struct Parallax
		{
			static const int id = 426;
		};
		struct Parallax_Factor
		{
			static const int id = 427;
			typedef mt::vec2f Type;
		};
		struct Param
		{
			static const int id = 115;
		};
		struct Param_Name
		{
			static const int id = 82;
			typedef Menge::ConstString Type;
		};
		struct Param_Value
		{
			static const int id = 84;
			typedef Menge::String Type;
		};
		struct Param_Value1
		{
			static const int id = 86;
			typedef Menge::String Type;
		};
		struct Param_Value10
		{
			static const int id = 104;
			typedef Menge::String Type;
		};
		struct Param_Value11
		{
			static const int id = 106;
			typedef Menge::String Type;
		};
		struct Param_Value12
		{
			static const int id = 108;
			typedef Menge::String Type;
		};
		struct Param_Value13
		{
			static const int id = 110;
			typedef Menge::String Type;
		};
		struct Param_Value14
		{
			static const int id = 112;
			typedef Menge::String Type;
		};
		struct Param_Value15
		{
			static const int id = 114;
			typedef Menge::String Type;
		};
		struct Param_Value16
		{
			static const int id = 116;
			typedef Menge::String Type;
		};
		struct Param_Value2
		{
			static const int id = 88;
			typedef Menge::String Type;
		};
		struct Param_Value3
		{
			static const int id = 90;
			typedef Menge::String Type;
		};
		struct Param_Value4
		{
			static const int id = 92;
			typedef Menge::String Type;
		};
		struct Param_Value5
		{
			static const int id = 94;
			typedef Menge::String Type;
		};
		struct Param_Value6
		{
			static const int id = 96;
			typedef Menge::String Type;
		};
		struct Param_Value7
		{
			static const int id = 98;
			typedef Menge::String Type;
		};
		struct Param_Value8
		{
			static const int id = 100;
			typedef Menge::String Type;
		};
		struct Param_Value9
		{
			static const int id = 102;
			typedef Menge::String Type;
		};
		struct Params
		{
			static const int id = 79;
		};
		struct Params_Path
		{
			static const int id = 80;
			typedef Menge::String Type;
		};
		struct PercentVisibility
		{
			static const int id = 210;
		};
		struct PercentVisibility_Value
		{
			static const int id = 211;
			typedef mt::vec4f Type;
		};
		struct PersonalityModule
		{
			static const int id = 23;
		};
		struct PersonalityModule_Value
		{
			static const int id = 24;
			typedef Menge::ConstString Type;
		};
		struct PhysicCanSleep
		{
			static const int id = 447;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 448;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 445;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 446;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 121;
		};
		struct PlayerCounter_Value
		{
			static const int id = 122;
			typedef int Type;
		};
		struct Point
		{
			static const int id = 148;
		};
		struct Point_Value
		{
			static const int id = 149;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 147;
		};
		struct Position
		{
			static const int id = 196;
		};
		struct Position_Value
		{
			static const int id = 197;
			typedef mt::vec2f Type;
		};
		struct Project
		{
			static const int id = 59;
		};
		struct Project_Name
		{
			static const int id = 60;
			typedef Menge::ConstString Type;
		};
		struct Radius
		{
			static const int id = 194;
		};
		struct Radius_Value
		{
			static const int id = 195;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 143;
		};
		struct RandomStart_Value
		{
			static const int id = 144;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 428;
		};
		struct RenderArea_Value
		{
			static const int id = 429;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 451;
		};
		struct RenderTarget_Name
		{
			static const int id = 450;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 452;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 434;
		};
		struct RenderTargetName_Value
		{
			static const int id = 435;
			typedef Menge::ConstString Type;
		};
		struct RenderViewport
		{
			static const int id = 430;
		};
		struct RenderViewport_Value
		{
			static const int id = 431;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 420;
		};
		struct Resource_Name
		{
			static const int id = 419;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 66;
			typedef Menge::String Type;
		};
		struct Resource_Type
		{
			static const int id = 421;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 239;
			typedef Menge::ConstString Type;
		};
		struct ResourceGlyph
		{
			static const int id = 267;
		};
		struct ResourceGlyph_Name
		{
			static const int id = 268;
			typedef Menge::ConstString Type;
		};
		struct ResourceImage
		{
			static const int id = 242;
		};
		struct ResourceImage_Name
		{
			static const int id = 243;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack
		{
			static const int id = 49;
		};
		struct ResourcePack_Description
		{
			static const int id = 48;
			typedef Menge::String Type;
		};
		struct ResourcePack_Name
		{
			static const int id = 44;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack_Path
		{
			static const int id = 46;
			typedef Menge::String Type;
		};
		struct ResourcePack_PreLoad
		{
			static const int id = 50;
			typedef bool Type;
		};
		struct ResourceResolution
		{
			static const int id = 17;
		};
		struct ResourceResolution_Value
		{
			static const int id = 18;
			typedef Menge::Resolution Type;
		};
		struct ResourceSequence
		{
			static const int id = 244;
		};
		struct ResourceSequence_Name
		{
			static const int id = 245;
			typedef Menge::ConstString Type;
		};
		struct ResourceSound
		{
			static const int id = 248;
		};
		struct ResourceSound_Name
		{
			static const int id = 249;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 361;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 362;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 246;
		};
		struct ResourceVideo_Name
		{
			static const int id = 247;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 486;
		};
		struct Resources_Path
		{
			static const int id = 487;
			typedef Menge::String Type;
		};
		struct Restitution
		{
			static const int id = 174;
		};
		struct Restitution_Value
		{
			static const int id = 175;
			typedef float Type;
		};
		struct Right
		{
			static const int id = 399;
		};
		struct Right_Codec
		{
			static const int id = 400;
			typedef Menge::ConstString Type;
		};
		struct Right_Image
		{
			static const int id = 398;
			typedef Menge::ConstString Type;
		};
		struct RightAlign
		{
			static const int id = 218;
		};
		struct RightAlign_Value
		{
			static const int id = 219;
			typedef bool Type;
		};
		struct RightBottom
		{
			static const int id = 403;
		};
		struct RightBottom_Codec
		{
			static const int id = 404;
			typedef Menge::ConstString Type;
		};
		struct RightBottom_Image
		{
			static const int id = 402;
			typedef Menge::ConstString Type;
		};
		struct RightTop
		{
			static const int id = 395;
		};
		struct RightTop_Codec
		{
			static const int id = 396;
			typedef Menge::ConstString Type;
		};
		struct RightTop_Image
		{
			static const int id = 394;
			typedef Menge::ConstString Type;
		};
		struct Scale
		{
			static const int id = 512;
		};
		struct Scale_Value
		{
			static const int id = 513;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 441;
		};
		struct Scene_Name
		{
			static const int id = 440;
			typedef Menge::ConstString Type;
		};
		struct Scene_Prototype
		{
			static const int id = 442;
			typedef Menge::ConstString Type;
		};
		struct Scenes
		{
			static const int id = 61;
		};
		struct Scenes_Path
		{
			static const int id = 62;
			typedef Menge::String Type;
		};
		struct Screensaver
		{
			static const int id = 514;
		};
		struct Screensaver_Name
		{
			static const int id = 515;
			typedef Menge::String Type;
		};
		struct Scripts
		{
			static const int id = 67;
		};
		struct Scripts_Path
		{
			static const int id = 68;
			typedef Menge::String Type;
		};
		struct Sequence
		{
			static const int id = 259;
		};
		struct Sequence_Delay
		{
			static const int id = 260;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 258;
			typedef size_t Type;
		};
		struct SequenceArray
		{
			static const int id = 263;
		};
		struct SequenceArray_Count
		{
			static const int id = 262;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 264;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 256;
		};
		struct Shape
		{
			static const int id = 479;
		};
		struct ShapeBox
		{
			static const int id = 481;
		};
		struct ShapeCircle
		{
			static const int id = 480;
		};
		struct Shuffle
		{
			static const int id = 350;
		};
		struct Shuffle_Value
		{
			static const int id = 351;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 424;
		};
		struct Size_Value
		{
			static const int id = 425;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 166;
		};
		struct StartPosition_Value
		{
			static const int id = 167;
			typedef float Type;
		};
		struct SubScene
		{
			static const int id = 518;
		};
		struct SubScene_Name
		{
			static const int id = 519;
			typedef Menge::ConstString Type;
		};
		struct Text
		{
			static const int id = 463;
		};
		struct Text_CharOffset
		{
			static const int id = 460;
			typedef float Type;
		};
		struct Text_Demand
		{
			static const int id = 78;
			typedef bool Type;
		};
		struct Text_File
		{
			static const int id = 76;
			typedef Menge::String Type;
		};
		struct Text_Font
		{
			static const int id = 464;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 454;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 462;
			typedef float Type;
		};
		struct Text_Name
		{
			static const int id = 74;
			typedef Menge::ConstString Type;
		};
		struct Text_Value
		{
			static const int id = 456;
			typedef Menge::String Type;
		};
		struct TextKey
		{
			static const int id = 457;
		};
		struct TextKey_Value
		{
			static const int id = 458;
			typedef Menge::ConstString Type;
		};
		struct Texts
		{
			static const int id = 469;
		};
		struct Texts_CharOffset
		{
			static const int id = 468;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 466;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 470;
			typedef float Type;
		};
		struct Texts_Path
		{
			static const int id = 72;
			typedef Menge::String Type;
		};
		struct Texture
		{
			static const int id = 319;
		};
		struct Texture_Color
		{
			static const int id = 320;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 318;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 377;
		};
		struct Tile_Image
		{
			static const int id = 374;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 376;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 378;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 372;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 370;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 379;
		};
		struct TileJunc_Image
		{
			static const int id = 380;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 363;
		};
		struct TileMap_File
		{
			static const int id = 364;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 237;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 367;
		};
		struct TileSet_File
		{
			static const int id = 368;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 365;
		};
		struct Tiles_Value
		{
			static const int id = 366;
			typedef size_t Type;
		};
		struct Title
		{
			static const int id = 15;
		};
		struct Title_Localized
		{
			static const int id = 16;
			typedef bool Type;
		};
		struct Title_Value
		{
			static const int id = 14;
			typedef Menge::ConstString Type;
		};
		struct Top
		{
			static const int id = 391;
		};
		struct Top_Codec
		{
			static const int id = 392;
			typedef Menge::ConstString Type;
		};
		struct Top_Image
		{
			static const int id = 390;
			typedef Menge::ConstString Type;
		};
		struct Track
		{
			static const int id = 357;
		};
		struct Track_Codec
		{
			static const int id = 356;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 354;
			typedef Menge::ConstString Type;
		};
		struct Track_Volume
		{
			static const int id = 358;
			typedef float Type;
		};
		struct Tracks
		{
			static const int id = 352;
		};
		struct Transformation
		{
			static const int id = 510;
		};
		struct Transformation_Angle
		{
			static const int id = 507;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 499;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 511;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 509;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 497;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 505;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 503;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 501;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 338;
		};
		struct Vertex_Coordinate
		{
			static const int id = 337;
			typedef mt::vec2f Type;
		};
		struct Vertex_UV
		{
			static const int id = 339;
			typedef mt::vec2f Type;
		};
		struct Vertex_Value
		{
			static const int id = 203;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 232;
		};
		struct VerticalAlign_Value
		{
			static const int id = 233;
			typedef int Type;
		};
		struct Vertices
		{
			static const int id = 333;
		};
		struct Vertices_Size
		{
			static const int id = 334;
			typedef size_t Type;
		};
		struct ViewportSize
		{
			static const int id = 436;
		};
		struct ViewportSize_Value
		{
			static const int id = 437;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 340;
		};
		struct Width_Value
		{
			static const int id = 341;
			typedef float Type;
		};
		struct Window
		{
			static const int id = 41;
		};
		struct Window_Bits
		{
			static const int id = 34;
			typedef int Type;
		};
		struct Window_Fullscreen
		{
			static const int id = 36;
			typedef bool Type;
		};
		struct Window_HasPanel
		{
			static const int id = 38;
			typedef bool Type;
		};
		struct Window_Size
		{
			static const int id = 32;
			typedef Menge::Resolution Type;
		};
		struct Window_TextureFiltering
		{
			static const int id = 42;
			typedef bool Type;
		};
		struct Window_VSync
		{
			static const int id = 40;
			typedef bool Type;
		};
		struct WindowResource
		{
			static const int id = 250;
		};
		struct WindowResource_Name
		{
			static const int id = 251;
			typedef Menge::ConstString Type;
		};
		struct WorkAreaDuration
		{
			static const int id = 586;
		};
		struct WorkAreaDuration_Value
		{
			static const int id = 587;
			typedef float Type;
		};
	}
}
