#	pragma once

namespace Menge
{
	namespace Protocol
	{
		struct AccountID
		{
			static const int id = 69;
		};
		struct AccountID_Value
		{
			static const int id = 70;
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
			static const int id = 135;
		};
		struct AllowSleep_Value
		{
			static const int id = 136;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 430;
		};
		struct Alpha_Value
		{
			static const int id = 431;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 103;
		};
		struct AlphaTest_Value
		{
			static const int id = 104;
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
			static const int id = 105;
		};
		struct Amplitude_Value
		{
			static const int id = 106;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 149;
		};
		struct Angle_Value
		{
			static const int id = 150;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 133;
		};
		struct AngularDamping_Value
		{
			static const int id = 134;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 85;
		};
		struct Animation_Name
		{
			static const int id = 86;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 398;
		};
		struct Arrow
		{
			static const int id = 404;
		};
		struct Arrow_Name
		{
			static const int id = 405;
			typedef Menge::String Type;
		};
		struct Arrows
		{
			static const int id = 65;
		};
		struct Arrows_Path
		{
			static const int id = 66;
			typedef Menge::ConstString Type;
		};
		struct AutoPlay
		{
			static const int id = 111;
		};
		struct AutoPlay_Value
		{
			static const int id = 112;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 89;
		};
		struct AutoStart_Value
		{
			static const int id = 90;
			typedef bool Type;
		};
		struct Background
		{
			static const int id = 299;
		};
		struct Background_Codec
		{
			static const int id = 300;
			typedef Menge::ConstString Type;
		};
		struct Background_Image
		{
			static const int id = 298;
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
		struct BlendDest
		{
			static const int id = 163;
		};
		struct BlendDest_Value
		{
			static const int id = 164;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 161;
		};
		struct BlendSource_Value
		{
			static const int id = 162;
			typedef int Type;
		};
		struct Bottom
		{
			static const int id = 323;
		};
		struct Bottom_Codec
		{
			static const int id = 324;
			typedef Menge::ConstString Type;
		};
		struct Bottom_Image
		{
			static const int id = 322;
			typedef Menge::ConstString Type;
		};
		struct CategoryBits
		{
			static const int id = 127;
		};
		struct CategoryBits_Value
		{
			static const int id = 128;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 391;
		};
		struct Cell_X
		{
			static const int id = 390;
			typedef std::size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 392;
			typedef std::size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 157;
		};
		struct CenterAlign_Value
		{
			static const int id = 158;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 226;
		};
		struct Char_code
		{
			static const int id = 223;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 227;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 225;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 221;
			typedef int Type;
		};
		struct CharOffset
		{
			static const int id = 175;
		};
		struct CharOffset_Value
		{
			static const int id = 176;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 93;
		};
		struct ClickOffset_Value
		{
			static const int id = 94;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 197;
		};
		struct ClientSize_Value
		{
			static const int id = 198;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 199;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 200;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 125;
		};
		struct CollisionMask_Value
		{
			static const int id = 126;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 83;
		};
		struct Color_Value
		{
			static const int id = 84;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 262;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 263;
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
			static const int id = 264;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 265;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 333;
		};
		struct Default
		{
			static const int id = 400;
		};
		struct DefaultAccountID
		{
			static const int id = 71;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 72;
			typedef Menge::String Type;
		};
		struct DefaultArrow
		{
			static const int id = 27;
		};
		struct DefaultArrow_Value
		{
			static const int id = 28;
			typedef Menge::ConstString Type;
		};
		struct Density
		{
			static const int id = 119;
		};
		struct Density_Value
		{
			static const int id = 120;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 260;
		};
		struct Depth_Value
		{
			static const int id = 261;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 189;
		};
		struct Edge_Layer
		{
			static const int id = 190;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 109;
		};
		struct Emitter_Name
		{
			static const int id = 110;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 117;
		};
		struct EmitterRelative_Value
		{
			static const int id = 118;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 79;
		};
		struct Enable_Value
		{
			static const int id = 80;
			typedef bool Type;
		};
		struct Entities
		{
			static const int id = 59;
		};
		struct Entities_Path
		{
			static const int id = 60;
			typedef Menge::ConstString Type;
		};
		struct Entity
		{
			static const int id = 408;
		};
		struct Entity_Name
		{
			static const int id = 407;
			typedef Menge::ConstString Type;
		};
		struct Entity_Type
		{
			static const int id = 409;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 179;
		};
		struct FieldSize_Value
		{
			static const int id = 180;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 385;
		};
		struct File_Alpha
		{
			static const int id = 241;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 386;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 243;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 237;
			typedef mt::vec2f Type;
		};
		struct File_Offset
		{
			static const int id = 235;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 384;
			typedef Menge::ConstString Type;
		};
		struct File_Size
		{
			static const int id = 239;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 247;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 245;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 233;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 249;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 251;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 230;
		};
		struct Filter_Value
		{
			static const int id = 231;
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
			static const int id = 139;
		};
		struct FixedRotation_Value
		{
			static const int id = 140;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 210;
		};
		struct Folder_Path
		{
			static const int id = 211;
			typedef Menge::String Type;
		};
		struct Font
		{
			static const int id = 218;
		};
		struct Font_Name
		{
			static const int id = 166;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 219;
			typedef float Type;
		};
		struct Fontdef
		{
			static const int id = 212;
		};
		struct Fontdef_Path
		{
			static const int id = 213;
			typedef Menge::String Type;
		};
		struct Frame
		{
			static const int id = 387;
		};
		struct Frame_UV
		{
			static const int id = 388;
			typedef mt::vec4f Type;
		};
		struct Frequency
		{
			static const int id = 107;
		};
		struct Frequency_Value
		{
			static const int id = 108;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 121;
		};
		struct Friction_Value
		{
			static const int id = 122;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 399;
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
			static const int id = 393;
		};
		struct Glyph_Resource
		{
			static const int id = 394;
			typedef Menge::ConstString Type;
		};
		struct Gravity2D
		{
			static const int id = 357;
		};
		struct Gravity2D_Value
		{
			static const int id = 358;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 348;
		};
		struct GridSize_Value
		{
			static const int id = 349;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 129;
		};
		struct GroupIndex_Value
		{
			static const int id = 130;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 153;
		};
		struct HeadMode_Value
		{
			static const int id = 154;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 147;
		};
		struct Height_Value
		{
			static const int id = 148;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 81;
		};
		struct Hide_Value
		{
			static const int id = 82;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 183;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 184;
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
			static const int id = 214;
		};
		struct Image_Path
		{
			static const int id = 215;
			typedef Menge::String Type;
		};
		struct ImageAlpha
		{
			static const int id = 155;
		};
		struct ImageAlpha_Name
		{
			static const int id = 156;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 101;
		};
		struct ImageIndex_Value
		{
			static const int id = 102;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 99;
		};
		struct ImageMap_Name
		{
			static const int id = 100;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 228;
		};
		struct ImageResource_Name
		{
			static const int id = 229;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 137;
		};
		struct IsBullet_Value
		{
			static const int id = 138;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 141;
		};
		struct IsSensor_Value
		{
			static const int id = 142;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 275;
		};
		struct IsStreamable_Value
		{
			static const int id = 276;
			typedef bool Type;
		};
		struct LanguagePack
		{
			static const int id = 55;
		};
		struct LanguagePack_Description
		{
			static const int id = 54;
			typedef Menge::String Type;
		};
		struct LanguagePack_Name
		{
			static const int id = 50;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Path
		{
			static const int id = 52;
			typedef Menge::String Type;
		};
		struct LanguagePack_PreLoad
		{
			static const int id = 56;
			typedef bool Type;
		};
		struct Left
		{
			static const int id = 331;
		};
		struct Left_Codec
		{
			static const int id = 332;
			typedef Menge::ConstString Type;
		};
		struct Left_Image
		{
			static const int id = 330;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom
		{
			static const int id = 327;
		};
		struct LeftBottom_Codec
		{
			static const int id = 328;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom_Image
		{
			static const int id = 326;
			typedef Menge::ConstString Type;
		};
		struct LeftTop
		{
			static const int id = 303;
		};
		struct LeftTop_Codec
		{
			static const int id = 304;
			typedef Menge::ConstString Type;
		};
		struct LeftTop_Image
		{
			static const int id = 302;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 177;
		};
		struct LineOffset_Value
		{
			static const int id = 178;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 131;
		};
		struct LinearDamping_Value
		{
			static const int id = 132;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 266;
		};
		struct Loop_Value
		{
			static const int id = 267;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 113;
		};
		struct Looped_Value
		{
			static const int id = 114;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 87;
		};
		struct Looping_Value
		{
			static const int id = 88;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 338;
		};
		struct Main_Value
		{
			static const int id = 339;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 173;
		};
		struct MaxWidth_Value
		{
			static const int id = 174;
			typedef float Type;
		};
		struct Mesh
		{
			static const int id = 256;
		};
		struct Mesh_Name
		{
			static const int id = 257;
			typedef Menge::String Type;
		};
		struct Node
		{
			static const int id = 77;
		};
		struct Node_Name
		{
			static const int id = 76;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 78;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 171;
		};
		struct Outline_Value
		{
			static const int id = 172;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 169;
		};
		struct OutlineColor_Value
		{
			static const int id = 170;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 216;
		};
		struct OutlineImage_Path
		{
			static const int id = 217;
			typedef Menge::String Type;
		};
		struct Pak
		{
			static const int id = 401;
		};
		struct Parallax
		{
			static const int id = 342;
		};
		struct Parallax_Factor
		{
			static const int id = 343;
			typedef mt::vec2f Type;
		};
		struct PercentVisibility
		{
			static const int id = 159;
		};
		struct PercentVisibility_Value
		{
			static const int id = 160;
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
			static const int id = 361;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 362;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 359;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 360;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 73;
		};
		struct PlayerCounter_Value
		{
			static const int id = 74;
			typedef int Type;
		};
		struct Point
		{
			static const int id = 97;
		};
		struct Point_Value
		{
			static const int id = 98;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 95;
		};
		struct Polygon_Point
		{
			static const int id = 96;
			typedef mt::vec2f Type;
		};
		struct Position
		{
			static const int id = 145;
		};
		struct Position_Value
		{
			static const int id = 146;
			typedef mt::vec2f Type;
		};
		struct Radius
		{
			static const int id = 143;
		};
		struct Radius_Value
		{
			static const int id = 144;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 91;
		};
		struct RandomStart_Value
		{
			static const int id = 92;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 344;
		};
		struct RenderArea_Value
		{
			static const int id = 345;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 365;
		};
		struct RenderTarget_Name
		{
			static const int id = 364;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 366;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 350;
		};
		struct RenderTargetName_Value
		{
			static const int id = 351;
			typedef Menge::String Type;
		};
		struct RenderViewport
		{
			static const int id = 346;
		};
		struct RenderViewport_Value
		{
			static const int id = 347;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 336;
		};
		struct Resource_Name
		{
			static const int id = 335;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 62;
			typedef Menge::String Type;
		};
		struct Resource_Type
		{
			static const int id = 337;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 188;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack
		{
			static const int id = 47;
		};
		struct ResourcePack_Description
		{
			static const int id = 46;
			typedef Menge::String Type;
		};
		struct ResourcePack_Name
		{
			static const int id = 42;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack_Path
		{
			static const int id = 44;
			typedef Menge::String Type;
		};
		struct ResourcePack_PreLoad
		{
			static const int id = 48;
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
			static const int id = 193;
		};
		struct ResourceSound_Name
		{
			static const int id = 194;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 277;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 278;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 191;
		};
		struct ResourceVideo_Name
		{
			static const int id = 192;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 402;
		};
		struct Resources_Path
		{
			static const int id = 403;
			typedef Menge::String Type;
		};
		struct Restitution
		{
			static const int id = 123;
		};
		struct Restitution_Value
		{
			static const int id = 124;
			typedef float Type;
		};
		struct Right
		{
			static const int id = 315;
		};
		struct Right_Codec
		{
			static const int id = 316;
			typedef Menge::ConstString Type;
		};
		struct Right_Image
		{
			static const int id = 314;
			typedef Menge::ConstString Type;
		};
		struct RightAlign
		{
			static const int id = 167;
		};
		struct RightAlign_Value
		{
			static const int id = 168;
			typedef bool Type;
		};
		struct RightBottom
		{
			static const int id = 319;
		};
		struct RightBottom_Codec
		{
			static const int id = 320;
			typedef Menge::ConstString Type;
		};
		struct RightBottom_Image
		{
			static const int id = 318;
			typedef Menge::ConstString Type;
		};
		struct RightTop
		{
			static const int id = 311;
		};
		struct RightTop_Codec
		{
			static const int id = 312;
			typedef Menge::ConstString Type;
		};
		struct RightTop_Image
		{
			static const int id = 310;
			typedef Menge::ConstString Type;
		};
		struct Scale
		{
			static const int id = 426;
		};
		struct Scale_Value
		{
			static const int id = 427;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 356;
		};
		struct Scene_Name
		{
			static const int id = 355;
			typedef Menge::String Type;
		};
		struct Scenes
		{
			static const int id = 57;
		};
		struct Scenes_Path
		{
			static const int id = 58;
			typedef Menge::ConstString Type;
		};
		struct Screensaver
		{
			static const int id = 428;
		};
		struct Screensaver_Name
		{
			static const int id = 429;
			typedef Menge::String Type;
		};
		struct Scripts
		{
			static const int id = 63;
		};
		struct Scripts_Path
		{
			static const int id = 64;
			typedef Menge::String Type;
		};
		struct Sequence
		{
			static const int id = 204;
		};
		struct Sequence_Delay
		{
			static const int id = 205;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 203;
			typedef size_t Type;
		};
		struct SequenceArray
		{
			static const int id = 208;
		};
		struct SequenceArray_Count
		{
			static const int id = 207;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 209;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 201;
		};
		struct Shape
		{
			static const int id = 395;
		};
		struct ShapeBox
		{
			static const int id = 397;
		};
		struct ShapeCircle
		{
			static const int id = 396;
		};
		struct Shuffle
		{
			static const int id = 268;
		};
		struct Shuffle_Value
		{
			static const int id = 269;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 340;
		};
		struct Size_Value
		{
			static const int id = 341;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 115;
		};
		struct StartPosition_Value
		{
			static const int id = 116;
			typedef float Type;
		};
		struct Text
		{
			static const int id = 375;
		};
		struct Text_CharOffset
		{
			static const int id = 372;
			typedef float Type;
		};
		struct Text_Font
		{
			static const int id = 376;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 368;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 374;
			typedef float Type;
		};
		struct Text_Path
		{
			static const int id = 68;
			typedef Menge::String Type;
		};
		struct Text_Value
		{
			static const int id = 370;
			typedef Menge::String Type;
		};
		struct Texts
		{
			static const int id = 381;
		};
		struct Texts_CharOffset
		{
			static const int id = 380;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 378;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 382;
			typedef float Type;
		};
		struct Texture
		{
			static const int id = 254;
		};
		struct Texture_Color
		{
			static const int id = 255;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 253;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 293;
		};
		struct Tile_Image
		{
			static const int id = 290;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 292;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 294;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 288;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 286;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 295;
		};
		struct TileJunc_Image
		{
			static const int id = 296;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 279;
		};
		struct TileMap_File
		{
			static const int id = 280;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 186;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 283;
		};
		struct TileSet_File
		{
			static const int id = 284;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 281;
		};
		struct Tiles_Value
		{
			static const int id = 282;
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
			static const int id = 307;
		};
		struct Top_Codec
		{
			static const int id = 308;
			typedef Menge::ConstString Type;
		};
		struct Top_Image
		{
			static const int id = 306;
			typedef Menge::ConstString Type;
		};
		struct Track
		{
			static const int id = 273;
		};
		struct Track_Codec
		{
			static const int id = 274;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 272;
			typedef Menge::String Type;
		};
		struct Tracks
		{
			static const int id = 270;
		};
		struct Transformation
		{
			static const int id = 424;
		};
		struct Transformation_Angle
		{
			static const int id = 421;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 413;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 425;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 423;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 411;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 419;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 417;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 415;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 151;
		};
		struct Vertex_Value
		{
			static const int id = 152;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 181;
		};
		struct VerticalAlign_Value
		{
			static const int id = 182;
			typedef int Type;
		};
		struct ViewportSize
		{
			static const int id = 352;
		};
		struct ViewportSize_Value
		{
			static const int id = 353;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 258;
		};
		struct Width_Value
		{
			static const int id = 259;
			typedef float Type;
		};
		struct Window
		{
			static const int id = 39;
		};
		struct Window_Bits
		{
			static const int id = 32;
			typedef int Type;
		};
		struct Window_Fullscreen
		{
			static const int id = 34;
			typedef bool Type;
		};
		struct Window_HasPanel
		{
			static const int id = 36;
			typedef bool Type;
		};
		struct Window_Size
		{
			static const int id = 30;
			typedef Menge::Resolution Type;
		};
		struct Window_TextureFiltering
		{
			static const int id = 40;
			typedef bool Type;
		};
		struct Window_VSync
		{
			static const int id = 38;
			typedef bool Type;
		};
		struct WindowResource
		{
			static const int id = 195;
		};
		struct WindowResource_Name
		{
			static const int id = 196;
			typedef Menge::ConstString Type;
		};
	}
}
