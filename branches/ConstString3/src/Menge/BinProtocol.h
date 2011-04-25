#	pragma once

namespace Menge
{
	namespace Protocol
	{
		static const int version = 1303478911;
		struct AccountID
		{
			static const int id = 71;
		};
		struct AccountID_Value
		{
			static const int id = 72;
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
			static const int id = 141;
		};
		struct AllowSleep_Value
		{
			static const int id = 142;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 461;
		};
		struct Alpha_Value
		{
			static const int id = 462;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 109;
		};
		struct AlphaTest_Value
		{
			static const int id = 110;
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
			static const int id = 111;
		};
		struct Amplitude_Value
		{
			static const int id = 112;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 155;
		};
		struct Angle_Value
		{
			static const int id = 156;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 139;
		};
		struct AngularDamping_Value
		{
			static const int id = 140;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 91;
		};
		struct Animation_Name
		{
			static const int id = 92;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh
		{
			static const int id = 276;
		};
		struct AnimationMesh_In
		{
			static const int id = 275;
			typedef float Type;
		};
		struct AnimationMesh_Name
		{
			static const int id = 271;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh_Out
		{
			static const int id = 277;
			typedef float Type;
		};
		struct AnimationMesh_Texture
		{
			static const int id = 273;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 429;
		};
		struct Arrow
		{
			static const int id = 435;
		};
		struct Arrow_Name
		{
			static const int id = 436;
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
			static const int id = 117;
		};
		struct AutoPlay_Value
		{
			static const int id = 118;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 95;
		};
		struct AutoStart_Value
		{
			static const int id = 96;
			typedef bool Type;
		};
		struct Background
		{
			static const int id = 330;
		};
		struct Background_Codec
		{
			static const int id = 331;
			typedef Menge::ConstString Type;
		};
		struct Background_Image
		{
			static const int id = 329;
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
			static const int id = 77;
		};
		struct BinaryNode_Type
		{
			static const int id = 78;
			typedef Menge::ConstString Type;
		};
		struct BlendDest
		{
			static const int id = 169;
		};
		struct BlendDest_Value
		{
			static const int id = 170;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 167;
		};
		struct BlendSource_Value
		{
			static const int id = 168;
			typedef int Type;
		};
		struct Bottom
		{
			static const int id = 354;
		};
		struct Bottom_Codec
		{
			static const int id = 355;
			typedef Menge::ConstString Type;
		};
		struct Bottom_Image
		{
			static const int id = 353;
			typedef Menge::ConstString Type;
		};
		struct CategoryBits
		{
			static const int id = 133;
		};
		struct CategoryBits_Value
		{
			static const int id = 134;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 422;
		};
		struct Cell_X
		{
			static const int id = 421;
			typedef size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 423;
			typedef size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 163;
		};
		struct CenterAlign_Value
		{
			static const int id = 164;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 232;
		};
		struct Char_code
		{
			static const int id = 229;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 233;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 231;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 227;
			typedef int Type;
		};
		struct CharOffset
		{
			static const int id = 181;
		};
		struct CharOffset_Value
		{
			static const int id = 182;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 99;
		};
		struct ClickOffset_Value
		{
			static const int id = 100;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 203;
		};
		struct ClientSize_Value
		{
			static const int id = 204;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 205;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 206;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 131;
		};
		struct CollisionMask_Value
		{
			static const int id = 132;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 89;
		};
		struct Color_Value
		{
			static const int id = 90;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 291;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 292;
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
			static const int id = 293;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 294;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 364;
		};
		struct Default
		{
			static const int id = 431;
		};
		struct DefaultAccountID
		{
			static const int id = 73;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 74;
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
			static const int id = 527;
		};
		struct DefaultVolume_Value
		{
			static const int id = 528;
			typedef float Type;
		};
		struct Density
		{
			static const int id = 125;
		};
		struct Density_Value
		{
			static const int id = 126;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 289;
		};
		struct Depth_Value
		{
			static const int id = 290;
			typedef float Type;
		};
		struct Duration
		{
			static const int id = 529;
		};
		struct Duration_Value
		{
			static const int id = 530;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 195;
		};
		struct Edge_Layer
		{
			static const int id = 196;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 115;
		};
		struct Emitter_Name
		{
			static const int id = 116;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 123;
		};
		struct EmitterRelative_Value
		{
			static const int id = 124;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 85;
		};
		struct Enable_Value
		{
			static const int id = 86;
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
			static const int id = 439;
		};
		struct Entity_Name
		{
			static const int id = 438;
			typedef Menge::ConstString Type;
		};
		struct Entity_Prototype
		{
			static const int id = 440;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 185;
		};
		struct FieldSize_Value
		{
			static const int id = 186;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 262;
		};
		struct File_Alpha
		{
			static const int id = 253;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 243;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 255;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 249;
			typedef mt::vec2f Type;
		};
		struct File_Name
		{
			static const int id = 239;
			typedef Menge::ConstString Type;
		};
		struct File_Offset
		{
			static const int id = 247;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 241;
			typedef Menge::ConstString Type;
		};
		struct File_Size
		{
			static const int id = 251;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 259;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 257;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 245;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 261;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 263;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 236;
		};
		struct Filter_Value
		{
			static const int id = 237;
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
			static const int id = 145;
		};
		struct FixedRotation_Value
		{
			static const int id = 146;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 216;
		};
		struct Folder_Path
		{
			static const int id = 217;
			typedef Menge::ConstString Type;
		};
		struct Font
		{
			static const int id = 224;
		};
		struct Font_Name
		{
			static const int id = 172;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 225;
			typedef float Type;
		};
		struct Frame
		{
			static const int id = 418;
		};
		struct Frame_UV
		{
			static const int id = 419;
			typedef mt::vec4f Type;
		};
		struct Frequency
		{
			static const int id = 113;
		};
		struct Frequency_Value
		{
			static const int id = 114;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 127;
		};
		struct Friction_Value
		{
			static const int id = 128;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 430;
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
			static const int id = 424;
		};
		struct Glyph_Resource
		{
			static const int id = 425;
			typedef Menge::ConstString Type;
		};
		struct Gravity2D
		{
			static const int id = 390;
		};
		struct Gravity2D_Value
		{
			static const int id = 391;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 379;
		};
		struct GridSize_Value
		{
			static const int id = 380;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 135;
		};
		struct GroupIndex_Value
		{
			static const int id = 136;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 159;
		};
		struct HeadMode_Value
		{
			static const int id = 160;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 153;
		};
		struct Height_Value
		{
			static const int id = 154;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 87;
		};
		struct Hide_Value
		{
			static const int id = 88;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 189;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 190;
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
			static const int id = 220;
		};
		struct Image_Path
		{
			static const int id = 221;
			typedef Menge::ConstString Type;
		};
		struct ImageAlpha
		{
			static const int id = 161;
		};
		struct ImageAlpha_Name
		{
			static const int id = 162;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 107;
		};
		struct ImageIndex_Value
		{
			static const int id = 108;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 105;
		};
		struct ImageMap_Name
		{
			static const int id = 106;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 234;
		};
		struct ImageResource_Name
		{
			static const int id = 235;
			typedef Menge::ConstString Type;
		};
		struct Indices
		{
			static const int id = 278;
		};
		struct Indices_Value
		{
			static const int id = 279;
			typedef std::vector<uint16> Type;
		};
		struct Internal
		{
			static const int id = 533;
		};
		struct Internal_Group
		{
			static const int id = 534;
			typedef Menge::ConstString Type;
		};
		struct Internal_Name
		{
			static const int id = 532;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 143;
		};
		struct IsBullet_Value
		{
			static const int id = 144;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 147;
		};
		struct IsSensor_Value
		{
			static const int id = 148;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 306;
		};
		struct IsStreamable_Value
		{
			static const int id = 307;
			typedef bool Type;
		};
		struct KeyFrame2D
		{
			static const int id = 489;
		};
		struct KeyFrame2D_AnchorPoint
		{
			static const int id = 482;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Opacity
		{
			static const int id = 490;
			typedef float Type;
		};
		struct KeyFrame2D_Position
		{
			static const int id = 484;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Rotation
		{
			static const int id = 488;
			typedef float Type;
		};
		struct KeyFrame2D_Scale
		{
			static const int id = 486;
			typedef mt::vec2f Type;
		};
		struct KeyFrame3D
		{
			static const int id = 525;
		};
		struct KeyFrame3D_AnchorPoint
		{
			static const int id = 518;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Opacity
		{
			static const int id = 526;
			typedef float Type;
		};
		struct KeyFrame3D_Position
		{
			static const int id = 520;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Rotation
		{
			static const int id = 524;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Scale
		{
			static const int id = 522;
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
			static const int id = 362;
		};
		struct Left_Codec
		{
			static const int id = 363;
			typedef Menge::ConstString Type;
		};
		struct Left_Image
		{
			static const int id = 361;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom
		{
			static const int id = 358;
		};
		struct LeftBottom_Codec
		{
			static const int id = 359;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom_Image
		{
			static const int id = 357;
			typedef Menge::ConstString Type;
		};
		struct LeftTop
		{
			static const int id = 334;
		};
		struct LeftTop_Codec
		{
			static const int id = 335;
			typedef Menge::ConstString Type;
		};
		struct LeftTop_Image
		{
			static const int id = 333;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 183;
		};
		struct LineOffset_Value
		{
			static const int id = 184;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 137;
		};
		struct LinearDamping_Value
		{
			static const int id = 138;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 295;
		};
		struct Loop_Value
		{
			static const int id = 296;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 119;
		};
		struct Looped_Value
		{
			static const int id = 120;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 93;
		};
		struct Looping_Value
		{
			static const int id = 94;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 369;
		};
		struct Main_Value
		{
			static const int id = 370;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 179;
		};
		struct MaxWidth_Value
		{
			static const int id = 180;
			typedef float Type;
		};
		struct Model
		{
			static const int id = 268;
		};
		struct Model_Name
		{
			static const int id = 269;
			typedef Menge::ConstString Type;
		};
		struct Movie
		{
			static const int id = 465;
		};
		struct Movie_Name
		{
			static const int id = 466;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D
		{
			static const int id = 479;
		};
		struct MovieLayer2D_In
		{
			static const int id = 478;
			typedef float Type;
		};
		struct MovieLayer2D_Index
		{
			static const int id = 474;
			typedef size_t Type;
		};
		struct MovieLayer2D_Internal
		{
			static const int id = 476;
			typedef bool Type;
		};
		struct MovieLayer2D_Name
		{
			static const int id = 468;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_Out
		{
			static const int id = 480;
			typedef float Type;
		};
		struct MovieLayer2D_Parent
		{
			static const int id = 470;
			typedef size_t Type;
		};
		struct MovieLayer2D_Source
		{
			static const int id = 472;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D
		{
			static const int id = 515;
		};
		struct MovieLayer3D_CameraAspect
		{
			static const int id = 512;
			typedef float Type;
		};
		struct MovieLayer3D_CameraFOV
		{
			static const int id = 510;
			typedef float Type;
		};
		struct MovieLayer3D_CameraInterest
		{
			static const int id = 508;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_CameraPosition
		{
			static const int id = 506;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_Height
		{
			static const int id = 516;
			typedef float Type;
		};
		struct MovieLayer3D_In
		{
			static const int id = 502;
			typedef float Type;
		};
		struct MovieLayer3D_Index
		{
			static const int id = 498;
			typedef size_t Type;
		};
		struct MovieLayer3D_Internal
		{
			static const int id = 500;
			typedef bool Type;
		};
		struct MovieLayer3D_Name
		{
			static const int id = 492;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Out
		{
			static const int id = 504;
			typedef float Type;
		};
		struct MovieLayer3D_Parent
		{
			static const int id = 494;
			typedef size_t Type;
		};
		struct MovieLayer3D_Source
		{
			static const int id = 496;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Width
		{
			static const int id = 514;
			typedef float Type;
		};
		struct Node
		{
			static const int id = 83;
		};
		struct Node_Name
		{
			static const int id = 80;
			typedef Menge::ConstString Type;
		};
		struct Node_Tag
		{
			static const int id = 84;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 82;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 177;
		};
		struct Outline_Value
		{
			static const int id = 178;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 175;
		};
		struct OutlineColor_Value
		{
			static const int id = 176;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 222;
		};
		struct OutlineImage_Path
		{
			static const int id = 223;
			typedef Menge::ConstString Type;
		};
		struct Pak
		{
			static const int id = 432;
		};
		struct Parallax
		{
			static const int id = 373;
		};
		struct Parallax_Factor
		{
			static const int id = 374;
			typedef mt::vec2f Type;
		};
		struct PercentVisibility
		{
			static const int id = 165;
		};
		struct PercentVisibility_Value
		{
			static const int id = 166;
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
			static const int id = 394;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 395;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 392;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 393;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 75;
		};
		struct PlayerCounter_Value
		{
			static const int id = 76;
			typedef int Type;
		};
		struct Point
		{
			static const int id = 103;
		};
		struct Point_Value
		{
			static const int id = 104;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 101;
		};
		struct Polygon_Point
		{
			static const int id = 102;
			typedef mt::vec2f Type;
		};
		struct Position
		{
			static const int id = 151;
		};
		struct Position_Value
		{
			static const int id = 152;
			typedef mt::vec2f Type;
		};
		struct Radius
		{
			static const int id = 149;
		};
		struct Radius_Value
		{
			static const int id = 150;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 97;
		};
		struct RandomStart_Value
		{
			static const int id = 98;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 375;
		};
		struct RenderArea_Value
		{
			static const int id = 376;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 398;
		};
		struct RenderTarget_Name
		{
			static const int id = 397;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 399;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 381;
		};
		struct RenderTargetName_Value
		{
			static const int id = 382;
			typedef Menge::ConstString Type;
		};
		struct RenderViewport
		{
			static const int id = 377;
		};
		struct RenderViewport_Value
		{
			static const int id = 378;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 367;
		};
		struct Resource_Name
		{
			static const int id = 366;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 64;
			typedef Menge::String Type;
		};
		struct Resource_Type
		{
			static const int id = 368;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 194;
			typedef Menge::ConstString Type;
		};
		struct ResourceGlyph
		{
			static const int id = 218;
		};
		struct ResourceGlyph_Name
		{
			static const int id = 219;
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
			static const int id = 199;
		};
		struct ResourceSound_Name
		{
			static const int id = 200;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 308;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 309;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 197;
		};
		struct ResourceVideo_Name
		{
			static const int id = 198;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 433;
		};
		struct Resources_Path
		{
			static const int id = 434;
			typedef Menge::String Type;
		};
		struct Restitution
		{
			static const int id = 129;
		};
		struct Restitution_Value
		{
			static const int id = 130;
			typedef float Type;
		};
		struct Right
		{
			static const int id = 346;
		};
		struct Right_Codec
		{
			static const int id = 347;
			typedef Menge::ConstString Type;
		};
		struct Right_Image
		{
			static const int id = 345;
			typedef Menge::ConstString Type;
		};
		struct RightAlign
		{
			static const int id = 173;
		};
		struct RightAlign_Value
		{
			static const int id = 174;
			typedef bool Type;
		};
		struct RightBottom
		{
			static const int id = 350;
		};
		struct RightBottom_Codec
		{
			static const int id = 351;
			typedef Menge::ConstString Type;
		};
		struct RightBottom_Image
		{
			static const int id = 349;
			typedef Menge::ConstString Type;
		};
		struct RightTop
		{
			static const int id = 342;
		};
		struct RightTop_Codec
		{
			static const int id = 343;
			typedef Menge::ConstString Type;
		};
		struct RightTop_Image
		{
			static const int id = 341;
			typedef Menge::ConstString Type;
		};
		struct Scale
		{
			static const int id = 457;
		};
		struct Scale_Value
		{
			static const int id = 458;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 388;
		};
		struct Scene_Name
		{
			static const int id = 387;
			typedef Menge::ConstString Type;
		};
		struct Scene_Prototype
		{
			static const int id = 389;
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
			static const int id = 459;
		};
		struct Screensaver_Name
		{
			static const int id = 460;
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
			static const int id = 210;
		};
		struct Sequence_Delay
		{
			static const int id = 211;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 209;
			typedef size_t Type;
		};
		struct SequenceArray
		{
			static const int id = 214;
		};
		struct SequenceArray_Count
		{
			static const int id = 213;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 215;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 207;
		};
		struct Shape
		{
			static const int id = 426;
		};
		struct ShapeBox
		{
			static const int id = 428;
		};
		struct ShapeCircle
		{
			static const int id = 427;
		};
		struct Shuffle
		{
			static const int id = 297;
		};
		struct Shuffle_Value
		{
			static const int id = 298;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 371;
		};
		struct Size_Value
		{
			static const int id = 372;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 121;
		};
		struct StartPosition_Value
		{
			static const int id = 122;
			typedef float Type;
		};
		struct SubScene
		{
			static const int id = 463;
		};
		struct SubScene_Name
		{
			static const int id = 464;
			typedef Menge::ConstString Type;
		};
		struct Text
		{
			static const int id = 410;
		};
		struct Text_CharOffset
		{
			static const int id = 407;
			typedef float Type;
		};
		struct Text_Font
		{
			static const int id = 411;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 401;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 409;
			typedef float Type;
		};
		struct Text_Path
		{
			static const int id = 70;
			typedef Menge::String Type;
		};
		struct Text_Value
		{
			static const int id = 403;
			typedef Menge::String Type;
		};
		struct TextKey
		{
			static const int id = 404;
		};
		struct TextKey_Value
		{
			static const int id = 405;
			typedef Menge::ConstString Type;
		};
		struct Texts
		{
			static const int id = 416;
		};
		struct Texts_CharOffset
		{
			static const int id = 415;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 413;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 417;
			typedef float Type;
		};
		struct Texture
		{
			static const int id = 266;
		};
		struct Texture_Color
		{
			static const int id = 267;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 265;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 324;
		};
		struct Tile_Image
		{
			static const int id = 321;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 323;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 325;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 319;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 317;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 326;
		};
		struct TileJunc_Image
		{
			static const int id = 327;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 310;
		};
		struct TileMap_File
		{
			static const int id = 311;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 192;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 314;
		};
		struct TileSet_File
		{
			static const int id = 315;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 312;
		};
		struct Tiles_Value
		{
			static const int id = 313;
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
			static const int id = 338;
		};
		struct Top_Codec
		{
			static const int id = 339;
			typedef Menge::ConstString Type;
		};
		struct Top_Image
		{
			static const int id = 337;
			typedef Menge::ConstString Type;
		};
		struct Track
		{
			static const int id = 304;
		};
		struct Track_Codec
		{
			static const int id = 303;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 301;
			typedef Menge::ConstString Type;
		};
		struct Track_Volume
		{
			static const int id = 305;
			typedef float Type;
		};
		struct Tracks
		{
			static const int id = 299;
		};
		struct Transformation
		{
			static const int id = 455;
		};
		struct Transformation_Angle
		{
			static const int id = 452;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 444;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 456;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 454;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 442;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 450;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 448;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 446;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 285;
		};
		struct Vertex_Coordinate
		{
			static const int id = 284;
			typedef mt::vec2f Type;
		};
		struct Vertex_UV
		{
			static const int id = 286;
			typedef mt::vec2f Type;
		};
		struct Vertex_Value
		{
			static const int id = 158;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 187;
		};
		struct VerticalAlign_Value
		{
			static const int id = 188;
			typedef int Type;
		};
		struct Vertices
		{
			static const int id = 280;
		};
		struct Vertices_Size
		{
			static const int id = 281;
			typedef size_t Type;
		};
		struct ViewportSize
		{
			static const int id = 383;
		};
		struct ViewportSize_Value
		{
			static const int id = 384;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 287;
		};
		struct Width_Value
		{
			static const int id = 288;
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
			static const int id = 201;
		};
		struct WindowResource_Name
		{
			static const int id = 202;
			typedef Menge::ConstString Type;
		};
	}
}
