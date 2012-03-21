#	pragma once

namespace Menge
{
	namespace Protocol
	{
		static const int version = 1332330235;
		struct AccountID
		{
			static const int id = 125;
		};
		struct AccountID_Value
		{
			static const int id = 126;
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
			static const int id = 194;
		};
		struct AllowSleep_Value
		{
			static const int id = 195;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 534;
		};
		struct Alpha_Value
		{
			static const int id = 535;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 162;
		};
		struct AlphaTest_Value
		{
			static const int id = 163;
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
			static const int id = 164;
		};
		struct Amplitude_Value
		{
			static const int id = 165;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 208;
		};
		struct Angle_Value
		{
			static const int id = 209;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 192;
		};
		struct AngularDamping_Value
		{
			static const int id = 193;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 145;
		};
		struct Animation_Name
		{
			static const int id = 146;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh
		{
			static const int id = 343;
		};
		struct AnimationMesh_In
		{
			static const int id = 342;
			typedef float Type;
		};
		struct AnimationMesh_Name
		{
			static const int id = 338;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh_Out
		{
			static const int id = 344;
			typedef float Type;
		};
		struct AnimationMesh_Texture
		{
			static const int id = 340;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 500;
		};
		struct Arrow
		{
			static const int id = 506;
		};
		struct Arrow_Name
		{
			static const int id = 507;
			typedef Menge::ConstString Type;
		};
		struct Arrows
		{
			static const int id = 77;
		};
		struct Arrows_Path
		{
			static const int id = 78;
			typedef Menge::String Type;
		};
		struct AutoPlay
		{
			static const int id = 170;
		};
		struct AutoPlay_Value
		{
			static const int id = 171;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 149;
		};
		struct AutoStart_Value
		{
			static const int id = 150;
			typedef bool Type;
		};
		struct Background
		{
			static const int id = 397;
		};
		struct Background_Codec
		{
			static const int id = 398;
			typedef Menge::ConstString Type;
		};
		struct Background_Image
		{
			static const int id = 396;
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
			static const int id = 131;
		};
		struct BinaryNode_Type
		{
			static const int id = 132;
			typedef Menge::ConstString Type;
		};
		struct BlendDest
		{
			static const int id = 222;
		};
		struct BlendDest_Value
		{
			static const int id = 223;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 220;
		};
		struct BlendSource_Value
		{
			static const int id = 221;
			typedef int Type;
		};
		struct Bottom
		{
			static const int id = 421;
		};
		struct Bottom_Codec
		{
			static const int id = 422;
			typedef Menge::ConstString Type;
		};
		struct Bottom_Image
		{
			static const int id = 420;
			typedef Menge::ConstString Type;
		};
		struct CategoryBits
		{
			static const int id = 186;
		};
		struct CategoryBits_Value
		{
			static const int id = 187;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 493;
		};
		struct Cell_X
		{
			static const int id = 492;
			typedef size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 494;
			typedef size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 216;
		};
		struct CenterAlign_Value
		{
			static const int id = 217;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 289;
		};
		struct Char_code
		{
			static const int id = 286;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 290;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 288;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 284;
			typedef float Type;
		};
		struct CharOffset
		{
			static const int id = 234;
		};
		struct CharOffset_Value
		{
			static const int id = 235;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 153;
		};
		struct ClickOffset_Value
		{
			static const int id = 154;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 260;
		};
		struct ClientSize_Value
		{
			static const int id = 261;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 262;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 263;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 184;
		};
		struct CollisionMask_Value
		{
			static const int id = 185;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 143;
		};
		struct Color_Value
		{
			static const int id = 144;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 358;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 359;
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
			static const int id = 360;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 361;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 431;
		};
		struct Default
		{
			static const int id = 502;
		};
		struct DefaultAccountID
		{
			static const int id = 127;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 128;
			typedef Menge::String Type;
		};
		struct DefaultArrow
		{
			static const int id = 31;
		};
		struct DefaultArrow_Name
		{
			static const int id = 30;
			typedef Menge::ConstString Type;
		};
		struct DefaultArrow_Prototype
		{
			static const int id = 32;
			typedef Menge::ConstString Type;
		};
		struct DefaultVolume
		{
			static const int id = 604;
		};
		struct DefaultVolume_Value
		{
			static const int id = 605;
			typedef float Type;
		};
		struct Density
		{
			static const int id = 178;
		};
		struct Density_Value
		{
			static const int id = 179;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 356;
		};
		struct Depth_Value
		{
			static const int id = 357;
			typedef float Type;
		};
		struct Duration
		{
			static const int id = 606;
		};
		struct Duration_Value
		{
			static const int id = 607;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 248;
		};
		struct Edge_Layer
		{
			static const int id = 249;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 168;
		};
		struct Emitter_Name
		{
			static const int id = 169;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 176;
		};
		struct EmitterRelative_Value
		{
			static const int id = 177;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 139;
		};
		struct Enable_Value
		{
			static const int id = 140;
			typedef bool Type;
		};
		struct Entities
		{
			static const int id = 71;
		};
		struct Entities_Path
		{
			static const int id = 72;
			typedef Menge::String Type;
		};
		struct Entity
		{
			static const int id = 512;
		};
		struct Entity_Name
		{
			static const int id = 509;
			typedef Menge::ConstString Type;
		};
		struct Entity_Prototype
		{
			static const int id = 511;
			typedef Menge::ConstString Type;
		};
		struct Entity_Tag
		{
			static const int id = 513;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 238;
		};
		struct FieldSize_Value
		{
			static const int id = 239;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 329;
		};
		struct File_Alpha
		{
			static const int id = 320;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 310;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 322;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 316;
			typedef mt::vec2f Type;
		};
		struct File_Name
		{
			static const int id = 300;
			typedef Menge::ConstString Type;
		};
		struct File_Offset
		{
			static const int id = 314;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 306;
			typedef Menge::ConstString Type;
		};
		struct File_PathAlpha
		{
			static const int id = 304;
			typedef Menge::ConstString Type;
		};
		struct File_PathRGB
		{
			static const int id = 302;
			typedef Menge::ConstString Type;
		};
		struct File_Size
		{
			static const int id = 318;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 326;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 324;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 312;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 328;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 330;
			typedef bool Type;
		};
		struct File_isCombined
		{
			static const int id = 308;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 297;
		};
		struct Filter_Value
		{
			static const int id = 298;
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
			static const int id = 198;
		};
		struct FixedRotation_Value
		{
			static const int id = 199;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 273;
		};
		struct Folder_Path
		{
			static const int id = 274;
			typedef Menge::ConstString Type;
		};
		struct Font
		{
			static const int id = 281;
		};
		struct Font_Name
		{
			static const int id = 225;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 282;
			typedef float Type;
		};
		struct Frame
		{
			static const int id = 489;
		};
		struct Frame_UV
		{
			static const int id = 490;
			typedef mt::vec4f Type;
		};
		struct Frequency
		{
			static const int id = 166;
		};
		struct Frequency_Value
		{
			static const int id = 167;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 180;
		};
		struct Friction_Value
		{
			static const int id = 181;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 501;
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
			static const int id = 495;
		};
		struct Glyph_Resource
		{
			static const int id = 496;
			typedef Menge::ConstString Type;
		};
		struct Gravity2D
		{
			static const int id = 461;
		};
		struct Gravity2D_Value
		{
			static const int id = 462;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 446;
		};
		struct GridSize_Value
		{
			static const int id = 447;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 188;
		};
		struct GroupIndex_Value
		{
			static const int id = 189;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 212;
		};
		struct HeadMode_Value
		{
			static const int id = 213;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 206;
		};
		struct Height_Value
		{
			static const int id = 207;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 141;
		};
		struct Hide_Value
		{
			static const int id = 142;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 242;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 243;
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
			static const int id = 277;
		};
		struct Image_Path
		{
			static const int id = 278;
			typedef Menge::ConstString Type;
		};
		struct ImageAlpha
		{
			static const int id = 214;
		};
		struct ImageAlpha_Name
		{
			static const int id = 215;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 160;
		};
		struct ImageIndex_Value
		{
			static const int id = 161;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 158;
		};
		struct ImageMap_Name
		{
			static const int id = 159;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 295;
		};
		struct ImageResource_Name
		{
			static const int id = 296;
			typedef Menge::ConstString Type;
		};
		struct Indices
		{
			static const int id = 345;
		};
		struct Indices_Value
		{
			static const int id = 346;
			typedef std::vector<uint16> Type;
		};
		struct Internal
		{
			static const int id = 612;
		};
		struct Internal_Group
		{
			static const int id = 613;
			typedef Menge::ConstString Type;
		};
		struct Internal_Name
		{
			static const int id = 611;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 196;
		};
		struct IsBullet_Value
		{
			static const int id = 197;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 200;
		};
		struct IsSensor_Value
		{
			static const int id = 201;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 373;
		};
		struct IsStreamable_Value
		{
			static const int id = 374;
			typedef bool Type;
		};
		struct Kerning
		{
			static const int id = 293;
		};
		struct Kerning_advance
		{
			static const int id = 292;
			typedef float Type;
		};
		struct Kerning_id
		{
			static const int id = 294;
			typedef Menge::String Type;
		};
		struct KeyFrame2D
		{
			static const int id = 564;
		};
		struct KeyFrame2D_AnchorPoint
		{
			static const int id = 555;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Index
		{
			static const int id = 565;
			typedef size_t Type;
		};
		struct KeyFrame2D_Opacity
		{
			static const int id = 563;
			typedef float Type;
		};
		struct KeyFrame2D_Position
		{
			static const int id = 557;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Rotation
		{
			static const int id = 561;
			typedef float Type;
		};
		struct KeyFrame2D_Scale
		{
			static const int id = 559;
			typedef mt::vec2f Type;
		};
		struct KeyFrame3D
		{
			static const int id = 602;
		};
		struct KeyFrame3D_AnchorPoint
		{
			static const int id = 593;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Index
		{
			static const int id = 603;
			typedef size_t Type;
		};
		struct KeyFrame3D_Opacity
		{
			static const int id = 601;
			typedef float Type;
		};
		struct KeyFrame3D_Position
		{
			static const int id = 595;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Rotation
		{
			static const int id = 599;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Scale
		{
			static const int id = 597;
			typedef mt::vec2f Type;
		};
		struct LanguagePack
		{
			static const int id = 65;
		};
		struct LanguagePack_Description
		{
			static const int id = 64;
			typedef Menge::String Type;
		};
		struct LanguagePack_Locale
		{
			static const int id = 58;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Name
		{
			static const int id = 56;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Path
		{
			static const int id = 62;
			typedef Menge::String Type;
		};
		struct LanguagePack_Platform
		{
			static const int id = 60;
			typedef Menge::String Type;
		};
		struct LanguagePack_PreLoad
		{
			static const int id = 66;
			typedef bool Type;
		};
		struct Left
		{
			static const int id = 429;
		};
		struct Left_Codec
		{
			static const int id = 430;
			typedef Menge::ConstString Type;
		};
		struct Left_Image
		{
			static const int id = 428;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom
		{
			static const int id = 425;
		};
		struct LeftBottom_Codec
		{
			static const int id = 426;
			typedef Menge::ConstString Type;
		};
		struct LeftBottom_Image
		{
			static const int id = 424;
			typedef Menge::ConstString Type;
		};
		struct LeftTop
		{
			static const int id = 401;
		};
		struct LeftTop_Codec
		{
			static const int id = 402;
			typedef Menge::ConstString Type;
		};
		struct LeftTop_Image
		{
			static const int id = 400;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 236;
		};
		struct LineOffset_Value
		{
			static const int id = 237;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 190;
		};
		struct LinearDamping_Value
		{
			static const int id = 191;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 362;
		};
		struct Loop_Value
		{
			static const int id = 363;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 172;
		};
		struct Looped_Value
		{
			static const int id = 173;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 147;
		};
		struct Looping_Value
		{
			static const int id = 148;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 436;
		};
		struct Main_Value
		{
			static const int id = 437;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 232;
		};
		struct MaxWidth_Value
		{
			static const int id = 233;
			typedef float Type;
		};
		struct Model
		{
			static const int id = 335;
		};
		struct Model_Name
		{
			static const int id = 336;
			typedef Menge::ConstString Type;
		};
		struct Movie
		{
			static const int id = 538;
		};
		struct Movie_Name
		{
			static const int id = 539;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D
		{
			static const int id = 552;
		};
		struct MovieLayer2D_In
		{
			static const int id = 551;
			typedef float Type;
		};
		struct MovieLayer2D_Index
		{
			static const int id = 547;
			typedef size_t Type;
		};
		struct MovieLayer2D_Internal
		{
			static const int id = 549;
			typedef bool Type;
		};
		struct MovieLayer2D_Name
		{
			static const int id = 541;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_Out
		{
			static const int id = 553;
			typedef float Type;
		};
		struct MovieLayer2D_Parent
		{
			static const int id = 543;
			typedef size_t Type;
		};
		struct MovieLayer2D_Source
		{
			static const int id = 545;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D
		{
			static const int id = 590;
		};
		struct MovieLayer3D_CameraAspect
		{
			static const int id = 587;
			typedef float Type;
		};
		struct MovieLayer3D_CameraFOV
		{
			static const int id = 585;
			typedef float Type;
		};
		struct MovieLayer3D_CameraInterest
		{
			static const int id = 583;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_CameraPosition
		{
			static const int id = 581;
			typedef mt::vec3f Type;
		};
		struct MovieLayer3D_Height
		{
			static const int id = 591;
			typedef float Type;
		};
		struct MovieLayer3D_In
		{
			static const int id = 577;
			typedef float Type;
		};
		struct MovieLayer3D_Index
		{
			static const int id = 573;
			typedef size_t Type;
		};
		struct MovieLayer3D_Internal
		{
			static const int id = 575;
			typedef bool Type;
		};
		struct MovieLayer3D_Name
		{
			static const int id = 567;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Out
		{
			static const int id = 579;
			typedef float Type;
		};
		struct MovieLayer3D_Parent
		{
			static const int id = 569;
			typedef size_t Type;
		};
		struct MovieLayer3D_Source
		{
			static const int id = 571;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Width
		{
			static const int id = 589;
			typedef float Type;
		};
		struct Node
		{
			static const int id = 137;
		};
		struct Node_Name
		{
			static const int id = 134;
			typedef Menge::ConstString Type;
		};
		struct Node_Tag
		{
			static const int id = 138;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 136;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 230;
		};
		struct Outline_Value
		{
			static const int id = 231;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 228;
		};
		struct OutlineColor_Value
		{
			static const int id = 229;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 279;
		};
		struct OutlineImage_Path
		{
			static const int id = 280;
			typedef Menge::ConstString Type;
		};
		struct Pak
		{
			static const int id = 503;
		};
		struct Parallax
		{
			static const int id = 440;
		};
		struct Parallax_Factor
		{
			static const int id = 441;
			typedef mt::vec2f Type;
		};
		struct Param
		{
			static const int id = 123;
		};
		struct Param_Name
		{
			static const int id = 90;
			typedef Menge::ConstString Type;
		};
		struct Param_Value
		{
			static const int id = 92;
			typedef Menge::String Type;
		};
		struct Param_Value1
		{
			static const int id = 94;
			typedef Menge::String Type;
		};
		struct Param_Value10
		{
			static const int id = 112;
			typedef Menge::String Type;
		};
		struct Param_Value11
		{
			static const int id = 114;
			typedef Menge::String Type;
		};
		struct Param_Value12
		{
			static const int id = 116;
			typedef Menge::String Type;
		};
		struct Param_Value13
		{
			static const int id = 118;
			typedef Menge::String Type;
		};
		struct Param_Value14
		{
			static const int id = 120;
			typedef Menge::String Type;
		};
		struct Param_Value15
		{
			static const int id = 122;
			typedef Menge::String Type;
		};
		struct Param_Value16
		{
			static const int id = 124;
			typedef Menge::String Type;
		};
		struct Param_Value2
		{
			static const int id = 96;
			typedef Menge::String Type;
		};
		struct Param_Value3
		{
			static const int id = 98;
			typedef Menge::String Type;
		};
		struct Param_Value4
		{
			static const int id = 100;
			typedef Menge::String Type;
		};
		struct Param_Value5
		{
			static const int id = 102;
			typedef Menge::String Type;
		};
		struct Param_Value6
		{
			static const int id = 104;
			typedef Menge::String Type;
		};
		struct Param_Value7
		{
			static const int id = 106;
			typedef Menge::String Type;
		};
		struct Param_Value8
		{
			static const int id = 108;
			typedef Menge::String Type;
		};
		struct Param_Value9
		{
			static const int id = 110;
			typedef Menge::String Type;
		};
		struct Params
		{
			static const int id = 87;
		};
		struct Params_Path
		{
			static const int id = 88;
			typedef Menge::String Type;
		};
		struct PercentVisibility
		{
			static const int id = 218;
		};
		struct PercentVisibility_Value
		{
			static const int id = 219;
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
			static const int id = 465;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 466;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 463;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 464;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 129;
		};
		struct PlayerCounter_Value
		{
			static const int id = 130;
			typedef int Type;
		};
		struct Plugin
		{
			static const int id = 27;
		};
		struct Plugin_Name
		{
			static const int id = 28;
			typedef Menge::String Type;
		};
		struct Point
		{
			static const int id = 156;
		};
		struct Point_Value
		{
			static const int id = 157;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 155;
		};
		struct Position
		{
			static const int id = 204;
		};
		struct Position_Value
		{
			static const int id = 205;
			typedef mt::vec2f Type;
		};
		struct PositionOffset
		{
			static const int id = 452;
		};
		struct PositionOffset_Value
		{
			static const int id = 453;
			typedef mt::vec2f Type;
		};
		struct Project
		{
			static const int id = 67;
		};
		struct Project_Name
		{
			static const int id = 68;
			typedef Menge::ConstString Type;
		};
		struct Radius
		{
			static const int id = 202;
		};
		struct Radius_Value
		{
			static const int id = 203;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 151;
		};
		struct RandomStart_Value
		{
			static const int id = 152;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 442;
		};
		struct RenderArea_Value
		{
			static const int id = 443;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 469;
		};
		struct RenderTarget_Name
		{
			static const int id = 468;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 470;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 448;
		};
		struct RenderTargetName_Value
		{
			static const int id = 449;
			typedef Menge::ConstString Type;
		};
		struct RenderViewport
		{
			static const int id = 444;
		};
		struct RenderViewport_Value
		{
			static const int id = 445;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 434;
		};
		struct Resource_Name
		{
			static const int id = 433;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 74;
			typedef Menge::String Type;
		};
		struct Resource_Type
		{
			static const int id = 435;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 247;
			typedef Menge::ConstString Type;
		};
		struct ResourceGlyph
		{
			static const int id = 275;
		};
		struct ResourceGlyph_Name
		{
			static const int id = 276;
			typedef Menge::ConstString Type;
		};
		struct ResourceImage
		{
			static const int id = 250;
		};
		struct ResourceImage_Name
		{
			static const int id = 251;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack
		{
			static const int id = 53;
		};
		struct ResourcePack_Description
		{
			static const int id = 52;
			typedef Menge::String Type;
		};
		struct ResourcePack_Locale
		{
			static const int id = 48;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack_Name
		{
			static const int id = 46;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack_Path
		{
			static const int id = 50;
			typedef Menge::String Type;
		};
		struct ResourcePack_PreLoad
		{
			static const int id = 54;
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
			static const int id = 252;
		};
		struct ResourceSequence_Name
		{
			static const int id = 253;
			typedef Menge::ConstString Type;
		};
		struct ResourceSound
		{
			static const int id = 256;
		};
		struct ResourceSound_Name
		{
			static const int id = 257;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 375;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 376;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 254;
		};
		struct ResourceVideo_Name
		{
			static const int id = 255;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 504;
		};
		struct Resources_Path
		{
			static const int id = 505;
			typedef Menge::String Type;
		};
		struct Restitution
		{
			static const int id = 182;
		};
		struct Restitution_Value
		{
			static const int id = 183;
			typedef float Type;
		};
		struct Right
		{
			static const int id = 413;
		};
		struct Right_Codec
		{
			static const int id = 414;
			typedef Menge::ConstString Type;
		};
		struct Right_Image
		{
			static const int id = 412;
			typedef Menge::ConstString Type;
		};
		struct RightAlign
		{
			static const int id = 226;
		};
		struct RightAlign_Value
		{
			static const int id = 227;
			typedef bool Type;
		};
		struct RightBottom
		{
			static const int id = 417;
		};
		struct RightBottom_Codec
		{
			static const int id = 418;
			typedef Menge::ConstString Type;
		};
		struct RightBottom_Image
		{
			static const int id = 416;
			typedef Menge::ConstString Type;
		};
		struct RightTop
		{
			static const int id = 409;
		};
		struct RightTop_Codec
		{
			static const int id = 410;
			typedef Menge::ConstString Type;
		};
		struct RightTop_Image
		{
			static const int id = 408;
			typedef Menge::ConstString Type;
		};
		struct Scale
		{
			static const int id = 530;
		};
		struct Scale_Value
		{
			static const int id = 531;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 459;
		};
		struct Scene_Name
		{
			static const int id = 456;
			typedef Menge::ConstString Type;
		};
		struct Scene_Prototype
		{
			static const int id = 460;
			typedef Menge::ConstString Type;
		};
		struct Scene_Script
		{
			static const int id = 458;
			typedef bool Type;
		};
		struct Scenes
		{
			static const int id = 69;
		};
		struct Scenes_Path
		{
			static const int id = 70;
			typedef Menge::String Type;
		};
		struct Screensaver
		{
			static const int id = 532;
		};
		struct Screensaver_Name
		{
			static const int id = 533;
			typedef Menge::String Type;
		};
		struct Scripts
		{
			static const int id = 75;
		};
		struct Scripts_Path
		{
			static const int id = 76;
			typedef Menge::String Type;
		};
		struct Sequence
		{
			static const int id = 267;
		};
		struct Sequence_Delay
		{
			static const int id = 268;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 266;
			typedef size_t Type;
		};
		struct SequenceArray
		{
			static const int id = 271;
		};
		struct SequenceArray_Count
		{
			static const int id = 270;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 272;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 264;
		};
		struct Shape
		{
			static const int id = 497;
		};
		struct ShapeBox
		{
			static const int id = 499;
		};
		struct ShapeCircle
		{
			static const int id = 498;
		};
		struct Shuffle
		{
			static const int id = 364;
		};
		struct Shuffle_Value
		{
			static const int id = 365;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 438;
		};
		struct Size_Value
		{
			static const int id = 439;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 174;
		};
		struct StartPosition_Value
		{
			static const int id = 175;
			typedef float Type;
		};
		struct SubScene
		{
			static const int id = 536;
		};
		struct SubScene_Name
		{
			static const int id = 537;
			typedef Menge::ConstString Type;
		};
		struct Text
		{
			static const int id = 481;
		};
		struct Text_CharOffset
		{
			static const int id = 478;
			typedef float Type;
		};
		struct Text_Demand
		{
			static const int id = 86;
			typedef bool Type;
		};
		struct Text_File
		{
			static const int id = 84;
			typedef Menge::String Type;
		};
		struct Text_Font
		{
			static const int id = 482;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 472;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 480;
			typedef float Type;
		};
		struct Text_Name
		{
			static const int id = 82;
			typedef Menge::ConstString Type;
		};
		struct Text_Value
		{
			static const int id = 474;
			typedef Menge::String Type;
		};
		struct TextKey
		{
			static const int id = 475;
		};
		struct TextKey_Value
		{
			static const int id = 476;
			typedef Menge::ConstString Type;
		};
		struct Texts
		{
			static const int id = 487;
		};
		struct Texts_CharOffset
		{
			static const int id = 486;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 484;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 488;
			typedef float Type;
		};
		struct Texts_Path
		{
			static const int id = 80;
			typedef Menge::String Type;
		};
		struct Texture
		{
			static const int id = 333;
		};
		struct Texture_Color
		{
			static const int id = 334;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 332;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 391;
		};
		struct Tile_Image
		{
			static const int id = 388;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 390;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 392;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 386;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 384;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 393;
		};
		struct TileJunc_Image
		{
			static const int id = 394;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 377;
		};
		struct TileMap_File
		{
			static const int id = 378;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 245;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 381;
		};
		struct TileSet_File
		{
			static const int id = 382;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 379;
		};
		struct Tiles_Value
		{
			static const int id = 380;
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
			static const int id = 405;
		};
		struct Top_Codec
		{
			static const int id = 406;
			typedef Menge::ConstString Type;
		};
		struct Top_Image
		{
			static const int id = 404;
			typedef Menge::ConstString Type;
		};
		struct Track
		{
			static const int id = 371;
		};
		struct Track_Codec
		{
			static const int id = 370;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 368;
			typedef Menge::ConstString Type;
		};
		struct Track_Volume
		{
			static const int id = 372;
			typedef float Type;
		};
		struct Tracks
		{
			static const int id = 366;
		};
		struct Transformation
		{
			static const int id = 528;
		};
		struct Transformation_Angle
		{
			static const int id = 525;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 517;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 529;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 527;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 515;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 523;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 521;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 519;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 352;
		};
		struct Vertex_Coordinate
		{
			static const int id = 351;
			typedef mt::vec2f Type;
		};
		struct Vertex_UV
		{
			static const int id = 353;
			typedef mt::vec2f Type;
		};
		struct Vertex_Value
		{
			static const int id = 211;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 240;
		};
		struct VerticalAlign_Value
		{
			static const int id = 241;
			typedef int Type;
		};
		struct Vertices
		{
			static const int id = 347;
		};
		struct Vertices_Size
		{
			static const int id = 348;
			typedef size_t Type;
		};
		struct ViewportSize
		{
			static const int id = 450;
		};
		struct ViewportSize_Value
		{
			static const int id = 451;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 354;
		};
		struct Width_Value
		{
			static const int id = 355;
			typedef float Type;
		};
		struct Window
		{
			static const int id = 43;
		};
		struct Window_Bits
		{
			static const int id = 36;
			typedef int Type;
		};
		struct Window_Fullscreen
		{
			static const int id = 38;
			typedef bool Type;
		};
		struct Window_HasPanel
		{
			static const int id = 40;
			typedef bool Type;
		};
		struct Window_Size
		{
			static const int id = 34;
			typedef Menge::Resolution Type;
		};
		struct Window_TextureFiltering
		{
			static const int id = 44;
			typedef bool Type;
		};
		struct Window_VSync
		{
			static const int id = 42;
			typedef bool Type;
		};
		struct WindowResource
		{
			static const int id = 258;
		};
		struct WindowResource_Name
		{
			static const int id = 259;
			typedef Menge::ConstString Type;
		};
		struct WorkAreaDuration
		{
			static const int id = 608;
		};
		struct WorkAreaDuration_Value
		{
			static const int id = 609;
			typedef float Type;
		};
	}
}
