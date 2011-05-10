#	pragma once

namespace Menge
{
	namespace Protocol
	{
		static const int version = 1304961836;
		struct AccountID
		{
			static const int id = 93;
		};
		struct AccountID_Value
		{
			static const int id = 94;
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
			static const int id = 163;
		};
		struct AllowSleep_Value
		{
			static const int id = 164;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 485;
		};
		struct Alpha_Value
		{
			static const int id = 486;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 131;
		};
		struct AlphaTest_Value
		{
			static const int id = 132;
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
			static const int id = 133;
		};
		struct Amplitude_Value
		{
			static const int id = 134;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 177;
		};
		struct Angle_Value
		{
			static const int id = 178;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 161;
		};
		struct AngularDamping_Value
		{
			static const int id = 162;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 113;
		};
		struct Animation_Name
		{
			static const int id = 114;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh
		{
			static const int id = 298;
		};
		struct AnimationMesh_In
		{
			static const int id = 297;
			typedef float Type;
		};
		struct AnimationMesh_Name
		{
			static const int id = 293;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh_Out
		{
			static const int id = 299;
			typedef float Type;
		};
		struct AnimationMesh_Texture
		{
			static const int id = 295;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 451;
		};
		struct Arrow
		{
			static const int id = 457;
		};
		struct Arrow_Name
		{
			static const int id = 458;
			typedef Menge::ConstString Type;
		};
		struct Arrows
		{
			static const int id = 67;
		};
		struct Arrows_Path
		{
			static const int id = 68;
			typedef Menge::ConstString Type;
		};
		struct AutoPlay
		{
			static const int id = 139;
		};
		struct AutoPlay_Value
		{
			static const int id = 140;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 117;
		};
		struct AutoStart_Value
		{
			static const int id = 118;
			typedef bool Type;
		};
		struct Background
		{
			static const int id = 352;
		};
		struct Background_Codec
		{
			static const int id = 353;
			typedef Menge::ConstString Type;
		};
		struct Background_Image
		{
			static const int id = 351;
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
			static const int id = 99;
		};
		struct BinaryNode_Type
		{
			static const int id = 100;
			typedef Menge::ConstString Type;
		};
		struct BlendDest
		{
			static const int id = 191;
		};
		struct BlendDest_Value
		{
			static const int id = 192;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 189;
		};
		struct BlendSource_Value
		{
			static const int id = 190;
			typedef int Type;
		};
		struct Bottom
		{
			static const int id = 376;
		};
		struct Bottom_Codec
		{
			static const int id = 377;
			typedef Menge::ConstString Type;
		};
		struct Bottom_Image
		{
			static const int id = 375;
			typedef Menge::ConstString Type;
		};
		struct CategoryBits
		{
			static const int id = 155;
		};
		struct CategoryBits_Value
		{
			static const int id = 156;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 444;
		};
		struct Cell_X
		{
			static const int id = 443;
			typedef size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 445;
			typedef size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 185;
		};
		struct CenterAlign_Value
		{
			static const int id = 186;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 254;
		};
		struct Char_code
		{
			static const int id = 251;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 255;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 253;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 249;
			typedef int Type;
		};
		struct CharOffset
		{
			static const int id = 203;
		};
		struct CharOffset_Value
		{
			static const int id = 204;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 121;
		};
		struct ClickOffset_Value
		{
			static const int id = 122;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 225;
		};
		struct ClientSize_Value
		{
			static const int id = 226;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 227;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 228;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 153;
		};
		struct CollisionMask_Value
		{
			static const int id = 154;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 111;
		};
		struct Color_Value
		{
			static const int id = 112;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 313;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 314;
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
			static const int id = 315;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 316;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 386;
		};
		struct Default
		{
			static const int id = 453;
		};
		struct DefaultAccountID
		{
			static const int id = 95;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 96;
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
			static const int id = 551;
		};
		struct DefaultVolume_Value
		{
			static const int id = 552;
			typedef float Type;
		};
		struct Density
		{
			static const int id = 147;
		};
		struct Density_Value
		{
			static const int id = 148;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 311;
		};
		struct Depth_Value
		{
			static const int id = 312;
			typedef float Type;
		};
		struct Duration
		{
			static const int id = 553;
		};
		struct Duration_Value
		{
			static const int id = 554;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 217;
		};
		struct Edge_Layer
		{
			static const int id = 218;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 137;
		};
		struct Emitter_Name
		{
			static const int id = 138;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 145;
		};
		struct EmitterRelative_Value
		{
			static const int id = 146;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 107;
		};
		struct Enable_Value
		{
			static const int id = 108;
			typedef bool Type;
		};
		struct Entities
		{
			static const int id = 61;
		};
		struct Entities_Path
		{
			static const int id = 62;
			typedef Menge::ConstString Type;
		};
		struct Entity
		{
			static const int id = 463;
		};
		struct Entity_Name
		{
			static const int id = 460;
			typedef Menge::ConstString Type;
		};
		struct Entity_Prototype
		{
			static const int id = 462;
			typedef Menge::ConstString Type;
		};
		struct Entity_Tag
		{
			static const int id = 464;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 207;
		};
		struct FieldSize_Value
		{
			static const int id = 208;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 284;
		};
		struct File_Alpha
		{
			static const int id = 275;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 265;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 277;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 271;
			typedef mt::vec2f Type;
		};
		struct File_Name
		{
			static const int id = 261;
			typedef Menge::ConstString Type;
		};
		struct File_Offset
		{
			static const int id = 269;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 263;
			typedef Menge::ConstString Type;
		};
		struct File_Size
		{
			static const int id = 273;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 281;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 279;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 267;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 283;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 285;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 258;
		};
		struct Filter_Value
		{
			static const int id = 259;
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
			static const int id = 167;
		};
		struct FixedRotation_Value
		{
			static const int id = 168;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 238;
		};
		struct Folder_Path
		{
			static const int id = 239;
			typedef Menge::ConstString Type;
		};
		struct Font
		{
			static const int id = 246;
		};
		struct Font_Name
		{
			static const int id = 194;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 247;
			typedef float Type;
		};
		struct Frame
		{
			static const int id = 440;
		};
		struct Frame_UV
		{
			static const int id = 441;
			typedef mt::vec4f Type;
		};
		struct Frequency
		{
			static const int id = 135;
		};
		struct Frequency_Value
		{
			static const int id = 136;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 149;
		};
		struct Friction_Value
		{
			static const int id = 150;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 452;
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
			static const int id = 446;
		};
		struct Glyph_Resource
		{
			static const int id = 447;
			typedef Menge::ConstString Type;
		};
		struct Gravity2D
		{
			static const int id = 412;
		};
		struct Gravity2D_Value
		{
			static const int id = 413;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 401;
		};
		struct GridSize_Value
		{
			static const int id = 402;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 157;
		};
		struct GroupIndex_Value
		{
			static const int id = 158;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 181;
		};
		struct HeadMode_Value
		{
			static const int id = 182;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 175;
		};
		struct Height_Value
		{
			static const int id = 176;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 109;
		};
		struct Hide_Value
		{
			static const int id = 110;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 211;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 212;
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
			static const int id = 242;
		};
		struct Image_Path
		{
			static const int id = 243;
			typedef Menge::ConstString Type;
		};
		struct ImageAlpha
		{
			static const int id = 183;
		};
		struct ImageAlpha_Name
		{
			static const int id = 184;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 129;
		};
		struct ImageIndex_Value
		{
			static const int id = 130;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 127;
		};
		struct ImageMap_Name
		{
			static const int id = 128;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 256;
		};
		struct ImageResource_Name
		{
			static const int id = 257;
			typedef Menge::ConstString Type;
		};
		struct Indices
		{
			static const int id = 300;
		};
		struct Indices_Value
		{
			static const int id = 301;
			typedef std::vector<uint16> Type;
		};
		struct Internal
		{
			static const int id = 557;
		};
		struct Internal_Group
		{
			static const int id = 558;
			typedef Menge::ConstString Type;
		};
		struct Internal_Name
		{
			static const int id = 556;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 165;
		};
		struct IsBullet_Value
		{
			static const int id = 166;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 169;
		};
		struct IsSensor_Value
		{
			static const int id = 170;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 328;
		};
		struct IsStreamable_Value
		{
			static const int id = 329;
			typedef bool Type;
		};
		struct KeyFrame2D
		{
			static const int id = 513;
		};
		struct KeyFrame2D_AnchorPoint
		{
			static const int id = 506;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Opacity
		{
			static const int id = 514;
			typedef float Type;
		};
		struct KeyFrame2D_Position
		{
			static const int id = 508;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Rotation
		{
			static const int id = 512;
			typedef float Type;
		};
		struct KeyFrame2D_Scale
		{
			static const int id = 510;
			typedef mt::vec2f Type;
		};
		struct KeyFrame3D
		{
			static const int id = 549;
		};
		struct KeyFrame3D_AnchorPoint
		{
			static const int id = 542;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Opacity
		{
			static const int id = 550;
			typedef float Type;
		};
		struct KeyFrame3D_Position
		{
			static const int id = 544;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Rotation
		{
			static const int id = 548;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Scale
		{
			static const int id = 546;
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
			static const int id = 384;
		};
		struct Left_Codec
		{
			static const int id = 385;
			typedef Menge::ConstString Type;
		};
		struct Left_Image
		{
			static const int id = 383;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom
		{
			static const int id = 380;
		};
		struct LeftBottom_Codec
		{
			static const int id = 381;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom_Image
		{
			static const int id = 379;
			typedef Menge::ConstString Type;
		};
		struct LeftTop
		{
			static const int id = 356;
		};
		struct LeftTop_Codec
		{
			static const int id = 357;
			typedef Menge::ConstString Type;
		};
		struct LeftTop_Image
		{
			static const int id = 355;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 205;
		};
		struct LineOffset_Value
		{
			static const int id = 206;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 159;
		};
		struct LinearDamping_Value
		{
			static const int id = 160;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 317;
		};
		struct Loop_Value
		{
			static const int id = 318;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 141;
		};
		struct Looped_Value
		{
			static const int id = 142;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 115;
		};
		struct Looping_Value
		{
			static const int id = 116;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 391;
		};
		struct Main_Value
		{
			static const int id = 392;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 201;
		};
		struct MaxWidth_Value
		{
			static const int id = 202;
			typedef float Type;
		};
		struct Model
		{
			static const int id = 290;
		};
		struct Model_Name
		{
			static const int id = 291;
			typedef Menge::ConstString Type;
		};
		struct Movie
		{
			static const int id = 489;
		};
		struct Movie_Name
		{
			static const int id = 490;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D
		{
			static const int id = 503;
		};
		struct MovieLayer2D_In
		{
			static const int id = 502;
			typedef float Type;
		};
		struct MovieLayer2D_Index
		{
			static const int id = 498;
			typedef size_t Type;
		};
		struct MovieLayer2D_Internal
		{
			static const int id = 500;
			typedef bool Type;
		};
		struct MovieLayer2D_Name
		{
			static const int id = 492;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_Out
		{
			static const int id = 504;
			typedef float Type;
		};
		struct MovieLayer2D_Parent
		{
			static const int id = 494;
			typedef size_t Type;
		};
		struct MovieLayer2D_Source
		{
			static const int id = 496;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D
		{
			static const int id = 539;
		};
		struct MovieLayer3D_CameraAspect
		{
			static const int id = 536;
			typedef float Type;
		};
		struct MovieLayer3D_CameraFOV
		{
			static const int id = 534;
			typedef float Type;
		};
		struct MovieLayer3D_CameraInterest
		{
			static const int id = 532;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_CameraPosition
		{
			static const int id = 530;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_Height
		{
			static const int id = 540;
			typedef float Type;
		};
		struct MovieLayer3D_In
		{
			static const int id = 526;
			typedef float Type;
		};
		struct MovieLayer3D_Index
		{
			static const int id = 522;
			typedef size_t Type;
		};
		struct MovieLayer3D_Internal
		{
			static const int id = 524;
			typedef bool Type;
		};
		struct MovieLayer3D_Name
		{
			static const int id = 516;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Out
		{
			static const int id = 528;
			typedef float Type;
		};
		struct MovieLayer3D_Parent
		{
			static const int id = 518;
			typedef size_t Type;
		};
		struct MovieLayer3D_Source
		{
			static const int id = 520;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Width
		{
			static const int id = 538;
			typedef float Type;
		};
		struct Node
		{
			static const int id = 105;
		};
		struct Node_Name
		{
			static const int id = 102;
			typedef Menge::ConstString Type;
		};
		struct Node_Tag
		{
			static const int id = 106;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 104;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 199;
		};
		struct Outline_Value
		{
			static const int id = 200;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 197;
		};
		struct OutlineColor_Value
		{
			static const int id = 198;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 244;
		};
		struct OutlineImage_Path
		{
			static const int id = 245;
			typedef Menge::ConstString Type;
		};
		struct Pak
		{
			static const int id = 454;
		};
		struct Parallax
		{
			static const int id = 395;
		};
		struct Parallax_Factor
		{
			static const int id = 396;
			typedef mt::vec2f Type;
		};
		struct Param
		{
			static const int id = 91;
		};
		struct Param_Key
		{
			static const int id = 76;
			typedef Menge::String Type;
		};
		struct Param_Name
		{
			static const int id = 74;
			typedef Menge::ConstString Type;
		};
		struct Param_Value
		{
			static const int id = 78;
			typedef Menge::String Type;
		};
		struct Param_Value1
		{
			static const int id = 80;
			typedef Menge::String Type;
		};
		struct Param_Value2
		{
			static const int id = 82;
			typedef Menge::String Type;
		};
		struct Param_Value3
		{
			static const int id = 84;
			typedef Menge::String Type;
		};
		struct Param_Value4
		{
			static const int id = 86;
			typedef Menge::String Type;
		};
		struct Param_Value5
		{
			static const int id = 88;
			typedef Menge::String Type;
		};
		struct Param_Value6
		{
			static const int id = 90;
			typedef Menge::String Type;
		};
		struct Param_Value7
		{
			static const int id = 92;
			typedef Menge::String Type;
		};
		struct Params
		{
			static const int id = 71;
		};
		struct Params_Path
		{
			static const int id = 72;
			typedef Menge::String Type;
		};
		struct PercentVisibility
		{
			static const int id = 187;
		};
		struct PercentVisibility_Value
		{
			static const int id = 188;
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
			static const int id = 416;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 417;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 414;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 415;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 97;
		};
		struct PlayerCounter_Value
		{
			static const int id = 98;
			typedef int Type;
		};
		struct Point
		{
			static const int id = 125;
		};
		struct Point_Value
		{
			static const int id = 126;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 123;
		};
		struct Polygon_Point
		{
			static const int id = 124;
			typedef mt::vec2f Type;
		};
		struct Position
		{
			static const int id = 173;
		};
		struct Position_Value
		{
			static const int id = 174;
			typedef mt::vec2f Type;
		};
		struct Radius
		{
			static const int id = 171;
		};
		struct Radius_Value
		{
			static const int id = 172;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 119;
		};
		struct RandomStart_Value
		{
			static const int id = 120;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 397;
		};
		struct RenderArea_Value
		{
			static const int id = 398;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 420;
		};
		struct RenderTarget_Name
		{
			static const int id = 419;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 421;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 403;
		};
		struct RenderTargetName_Value
		{
			static const int id = 404;
			typedef Menge::ConstString Type;
		};
		struct RenderViewport
		{
			static const int id = 399;
		};
		struct RenderViewport_Value
		{
			static const int id = 400;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 389;
		};
		struct Resource_Name
		{
			static const int id = 388;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 64;
			typedef Menge::String Type;
		};
		struct Resource_Type
		{
			static const int id = 390;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 216;
			typedef Menge::ConstString Type;
		};
		struct ResourceGlyph
		{
			static const int id = 240;
		};
		struct ResourceGlyph_Name
		{
			static const int id = 241;
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
		struct ResourceSound
		{
			static const int id = 221;
		};
		struct ResourceSound_Name
		{
			static const int id = 222;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 330;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 331;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 219;
		};
		struct ResourceVideo_Name
		{
			static const int id = 220;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 455;
		};
		struct Resources_Path
		{
			static const int id = 456;
			typedef Menge::String Type;
		};
		struct Restitution
		{
			static const int id = 151;
		};
		struct Restitution_Value
		{
			static const int id = 152;
			typedef float Type;
		};
		struct Right
		{
			static const int id = 368;
		};
		struct Right_Codec
		{
			static const int id = 369;
			typedef Menge::ConstString Type;
		};
		struct Right_Image
		{
			static const int id = 367;
			typedef Menge::ConstString Type;
		};
		struct RightAlign
		{
			static const int id = 195;
		};
		struct RightAlign_Value
		{
			static const int id = 196;
			typedef bool Type;
		};
		struct RightBottom
		{
			static const int id = 372;
		};
		struct RightBottom_Codec
		{
			static const int id = 373;
			typedef Menge::ConstString Type;
		};
		struct RightBottom_Image
		{
			static const int id = 371;
			typedef Menge::ConstString Type;
		};
		struct RightTop
		{
			static const int id = 364;
		};
		struct RightTop_Codec
		{
			static const int id = 365;
			typedef Menge::ConstString Type;
		};
		struct RightTop_Image
		{
			static const int id = 363;
			typedef Menge::ConstString Type;
		};
		struct Scale
		{
			static const int id = 481;
		};
		struct Scale_Value
		{
			static const int id = 482;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 410;
		};
		struct Scene_Name
		{
			static const int id = 409;
			typedef Menge::ConstString Type;
		};
		struct Scene_Prototype
		{
			static const int id = 411;
			typedef Menge::ConstString Type;
		};
		struct Scenes
		{
			static const int id = 59;
		};
		struct Scenes_Path
		{
			static const int id = 60;
			typedef Menge::ConstString Type;
		};
		struct Screensaver
		{
			static const int id = 483;
		};
		struct Screensaver_Name
		{
			static const int id = 484;
			typedef Menge::String Type;
		};
		struct Scripts
		{
			static const int id = 65;
		};
		struct Scripts_Path
		{
			static const int id = 66;
			typedef Menge::String Type;
		};
		struct Sequence
		{
			static const int id = 232;
		};
		struct Sequence_Delay
		{
			static const int id = 233;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 231;
			typedef size_t Type;
		};
		struct SequenceArray
		{
			static const int id = 236;
		};
		struct SequenceArray_Count
		{
			static const int id = 235;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 237;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 229;
		};
		struct Shape
		{
			static const int id = 448;
		};
		struct ShapeBox
		{
			static const int id = 450;
		};
		struct ShapeCircle
		{
			static const int id = 449;
		};
		struct Shuffle
		{
			static const int id = 319;
		};
		struct Shuffle_Value
		{
			static const int id = 320;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 393;
		};
		struct Size_Value
		{
			static const int id = 394;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 143;
		};
		struct StartPosition_Value
		{
			static const int id = 144;
			typedef float Type;
		};
		struct SubScene
		{
			static const int id = 487;
		};
		struct SubScene_Name
		{
			static const int id = 488;
			typedef Menge::ConstString Type;
		};
		struct Text
		{
			static const int id = 432;
		};
		struct Text_CharOffset
		{
			static const int id = 429;
			typedef float Type;
		};
		struct Text_Font
		{
			static const int id = 433;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 423;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 431;
			typedef float Type;
		};
		struct Text_Path
		{
			static const int id = 70;
			typedef Menge::String Type;
		};
		struct Text_Value
		{
			static const int id = 425;
			typedef Menge::String Type;
		};
		struct TextKey
		{
			static const int id = 426;
		};
		struct TextKey_Value
		{
			static const int id = 427;
			typedef Menge::ConstString Type;
		};
		struct Texts
		{
			static const int id = 438;
		};
		struct Texts_CharOffset
		{
			static const int id = 437;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 435;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 439;
			typedef float Type;
		};
		struct Texture
		{
			static const int id = 288;
		};
		struct Texture_Color
		{
			static const int id = 289;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 287;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 346;
		};
		struct Tile_Image
		{
			static const int id = 343;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 345;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 347;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 341;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 339;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 348;
		};
		struct TileJunc_Image
		{
			static const int id = 349;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 332;
		};
		struct TileMap_File
		{
			static const int id = 333;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 214;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 336;
		};
		struct TileSet_File
		{
			static const int id = 337;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 334;
		};
		struct Tiles_Value
		{
			static const int id = 335;
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
			static const int id = 360;
		};
		struct Top_Codec
		{
			static const int id = 361;
			typedef Menge::ConstString Type;
		};
		struct Top_Image
		{
			static const int id = 359;
			typedef Menge::ConstString Type;
		};
		struct Track
		{
			static const int id = 326;
		};
		struct Track_Codec
		{
			static const int id = 325;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 323;
			typedef Menge::ConstString Type;
		};
		struct Track_Volume
		{
			static const int id = 327;
			typedef float Type;
		};
		struct Tracks
		{
			static const int id = 321;
		};
		struct Transformation
		{
			static const int id = 479;
		};
		struct Transformation_Angle
		{
			static const int id = 476;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 468;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 480;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 478;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 466;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 474;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 472;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 470;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 307;
		};
		struct Vertex_Coordinate
		{
			static const int id = 306;
			typedef mt::vec2f Type;
		};
		struct Vertex_UV
		{
			static const int id = 308;
			typedef mt::vec2f Type;
		};
		struct Vertex_Value
		{
			static const int id = 180;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 209;
		};
		struct VerticalAlign_Value
		{
			static const int id = 210;
			typedef int Type;
		};
		struct Vertices
		{
			static const int id = 302;
		};
		struct Vertices_Size
		{
			static const int id = 303;
			typedef size_t Type;
		};
		struct ViewportSize
		{
			static const int id = 405;
		};
		struct ViewportSize_Value
		{
			static const int id = 406;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 309;
		};
		struct Width_Value
		{
			static const int id = 310;
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
			static const int id = 223;
		};
		struct WindowResource_Name
		{
			static const int id = 224;
			typedef Menge::ConstString Type;
		};
	}
}
