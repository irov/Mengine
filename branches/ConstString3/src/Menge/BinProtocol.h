#	pragma once

namespace Menge
{
	namespace Protocol
	{
		static const int version = 1341572834;
		struct AccountID
		{
			static const int id = 129;
		};
		struct AccountID_Value
		{
			static const int id = 130;
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
			static const int id = 198;
		};
		struct AllowSleep_Value
		{
			static const int id = 199;
			typedef bool Type;
		};
		struct Alpha
		{
			static const int id = 550;
		};
		struct Alpha_Value
		{
			static const int id = 551;
			typedef float Type;
		};
		struct AlphaTest
		{
			static const int id = 166;
		};
		struct AlphaTest_Value
		{
			static const int id = 167;
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
			static const int id = 168;
		};
		struct Amplitude_Value
		{
			static const int id = 169;
			typedef float Type;
		};
		struct Angle
		{
			static const int id = 212;
		};
		struct Angle_Value
		{
			static const int id = 213;
			typedef float Type;
		};
		struct AngularDamping
		{
			static const int id = 196;
		};
		struct AngularDamping_Value
		{
			static const int id = 197;
			typedef float Type;
		};
		struct Animation
		{
			static const int id = 149;
		};
		struct Animation_Name
		{
			static const int id = 150;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh
		{
			static const int id = 361;
		};
		struct AnimationMesh_In
		{
			static const int id = 360;
			typedef float Type;
		};
		struct AnimationMesh_Name
		{
			static const int id = 356;
			typedef Menge::ConstString Type;
		};
		struct AnimationMesh_Out
		{
			static const int id = 362;
			typedef float Type;
		};
		struct AnimationMesh_Texture
		{
			static const int id = 358;
			typedef Menge::ConstString Type;
		};
		struct Application
		{
			static const int id = 516;
		};
		struct Arrow
		{
			static const int id = 522;
		};
		struct Arrow_Name
		{
			static const int id = 523;
			typedef Menge::ConstString Type;
		};
		struct Arrows
		{
			static const int id = 81;
		};
		struct Arrows_Path
		{
			static const int id = 82;
			typedef Menge::WString Type;
		};
		struct AutoPlay
		{
			static const int id = 174;
		};
		struct AutoPlay_Value
		{
			static const int id = 175;
			typedef bool Type;
		};
		struct AutoStart
		{
			static const int id = 153;
		};
		struct AutoStart_Value
		{
			static const int id = 154;
			typedef bool Type;
		};
		struct BaseDir
		{
			static const int id = 1;
		};
		struct BaseDir_Value
		{
			static const int id = 2;
			typedef Menge::WString Type;
		};
		struct BinaryNode
		{
			static const int id = 135;
		};
		struct BinaryNode_Type
		{
			static const int id = 136;
			typedef Menge::ConstString Type;
		};
		struct BlendDest
		{
			static const int id = 226;
		};
		struct BlendDest_Value
		{
			static const int id = 227;
			typedef int Type;
		};
		struct BlendSource
		{
			static const int id = 224;
		};
		struct BlendSource_Value
		{
			static const int id = 225;
			typedef int Type;
		};
		struct CategoryBits
		{
			static const int id = 190;
		};
		struct CategoryBits_Value
		{
			static const int id = 191;
			typedef size_t Type;
		};
		struct Cell
		{
			static const int id = 509;
		};
		struct Cell_X
		{
			static const int id = 508;
			typedef size_t Type;
		};
		struct Cell_Y
		{
			static const int id = 510;
			typedef size_t Type;
		};
		struct CenterAlign
		{
			static const int id = 220;
		};
		struct CenterAlign_Value
		{
			static const int id = 221;
			typedef bool Type;
		};
		struct Char
		{
			static const int id = 305;
		};
		struct Char_code
		{
			static const int id = 302;
			typedef Menge::String Type;
		};
		struct Char_offset
		{
			static const int id = 306;
			typedef Menge::String Type;
		};
		struct Char_rect
		{
			static const int id = 304;
			typedef Menge::String Type;
		};
		struct Char_width
		{
			static const int id = 300;
			typedef float Type;
		};
		struct CharOffset
		{
			static const int id = 246;
		};
		struct CharOffset_Value
		{
			static const int id = 247;
			typedef float Type;
		};
		struct ClickOffset
		{
			static const int id = 157;
		};
		struct ClickOffset_Value
		{
			static const int id = 158;
			typedef mt::vec2f Type;
		};
		struct ClientSize
		{
			static const int id = 272;
		};
		struct ClientSize_Value
		{
			static const int id = 273;
			typedef mt::vec2f Type;
		};
		struct ClientSizeInTiles
		{
			static const int id = 274;
		};
		struct ClientSizeInTiles_Value
		{
			static const int id = 275;
			typedef mt::vec2f Type;
		};
		struct CollisionMask
		{
			static const int id = 188;
		};
		struct CollisionMask_Value
		{
			static const int id = 189;
			typedef size_t Type;
		};
		struct Color
		{
			static const int id = 147;
		};
		struct Color_Value
		{
			static const int id = 148;
			typedef Menge::ColourValue Type;
		};
		struct ConcaveGeometryFile
		{
			static const int id = 376;
		};
		struct ConcaveGeometryFile_Name
		{
			static const int id = 377;
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
			static const int id = 378;
		};
		struct ConvexGeometryFile_Name
		{
			static const int id = 379;
			typedef Menge::String Type;
		};
		struct DataBlock
		{
			static const int id = 447;
		};
		struct Default
		{
			static const int id = 518;
		};
		struct DefaultAccountID
		{
			static const int id = 131;
		};
		struct DefaultAccountID_Value
		{
			static const int id = 132;
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
			static const int id = 636;
		};
		struct DefaultVolume_Value
		{
			static const int id = 637;
			typedef float Type;
		};
		struct Density
		{
			static const int id = 182;
		};
		struct Density_Value
		{
			static const int id = 183;
			typedef float Type;
		};
		struct Depth
		{
			static const int id = 374;
		};
		struct Depth_Value
		{
			static const int id = 375;
			typedef float Type;
		};
		struct Duration
		{
			static const int id = 638;
		};
		struct Duration_Value
		{
			static const int id = 639;
			typedef float Type;
		};
		struct Edge
		{
			static const int id = 260;
		};
		struct Edge_Layer
		{
			static const int id = 261;
			typedef Menge::String Type;
		};
		struct Emitter
		{
			static const int id = 172;
		};
		struct Emitter_Name
		{
			static const int id = 173;
			typedef Menge::ConstString Type;
		};
		struct EmitterRelative
		{
			static const int id = 180;
		};
		struct EmitterRelative_Value
		{
			static const int id = 181;
			typedef bool Type;
		};
		struct Enable
		{
			static const int id = 143;
		};
		struct Enable_Value
		{
			static const int id = 144;
			typedef bool Type;
		};
		struct Entities
		{
			static const int id = 75;
		};
		struct Entities_Path
		{
			static const int id = 76;
			typedef Menge::WString Type;
		};
		struct Entity
		{
			static const int id = 528;
		};
		struct Entity_Name
		{
			static const int id = 525;
			typedef Menge::ConstString Type;
		};
		struct Entity_Prototype
		{
			static const int id = 527;
			typedef Menge::ConstString Type;
		};
		struct Entity_Tag
		{
			static const int id = 529;
			typedef Menge::ConstString Type;
		};
		struct FieldSize
		{
			static const int id = 250;
		};
		struct FieldSize_Value
		{
			static const int id = 251;
			typedef mt::vec2f Type;
		};
		struct File
		{
			static const int id = 347;
		};
		struct File_Alpha
		{
			static const int id = 338;
			typedef bool Type;
		};
		struct File_Codec
		{
			static const int id = 326;
			typedef Menge::ConstString Type;
		};
		struct File_Converter
		{
			static const int id = 328;
			typedef Menge::ConstString Type;
		};
		struct File_From
		{
			static const int id = 340;
			typedef int Type;
		};
		struct File_MaxSize
		{
			static const int id = 334;
			typedef mt::vec2f Type;
		};
		struct File_Name
		{
			static const int id = 316;
			typedef Menge::ConstString Type;
		};
		struct File_Offset
		{
			static const int id = 332;
			typedef mt::vec2f Type;
		};
		struct File_Path
		{
			static const int id = 322;
			typedef Menge::WString Type;
		};
		struct File_PathAlpha
		{
			static const int id = 320;
			typedef Menge::WString Type;
		};
		struct File_PathRGB
		{
			static const int id = 318;
			typedef Menge::WString Type;
		};
		struct File_Size
		{
			static const int id = 336;
			typedef mt::vec2f Type;
		};
		struct File_Step
		{
			static const int id = 344;
			typedef int Type;
		};
		struct File_To
		{
			static const int id = 342;
			typedef int Type;
		};
		struct File_UV
		{
			static const int id = 330;
			typedef mt::vec4f Type;
		};
		struct File_WrapX
		{
			static const int id = 346;
			typedef bool Type;
		};
		struct File_WrapY
		{
			static const int id = 348;
			typedef bool Type;
		};
		struct File_isCombined
		{
			static const int id = 324;
			typedef bool Type;
		};
		struct Filter
		{
			static const int id = 313;
		};
		struct Filter_Value
		{
			static const int id = 314;
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
			static const int id = 202;
		};
		struct FixedRotation_Value
		{
			static const int id = 203;
			typedef bool Type;
		};
		struct Folder
		{
			static const int id = 289;
		};
		struct Folder_Path
		{
			static const int id = 290;
			typedef Menge::WString Type;
		};
		struct Font
		{
			static const int id = 236;
		};
		struct Font_Name
		{
			static const int id = 229;
			typedef Menge::ConstString Type;
		};
		struct Font_family
		{
			static const int id = 233;
			typedef Menge::ConstString Type;
		};
		struct Font_height
		{
			static const int id = 235;
			typedef float Type;
		};
		struct Font_size
		{
			static const int id = 231;
			typedef size_t Type;
		};
		struct Font_style
		{
			static const int id = 237;
			typedef Menge::ConstString Type;
		};
		struct Frame
		{
			static const int id = 505;
		};
		struct Frame_UV
		{
			static const int id = 506;
			typedef mt::vec4f Type;
		};
		struct FrameDuration
		{
			static const int id = 640;
		};
		struct FrameDuration_Value
		{
			static const int id = 641;
			typedef float Type;
		};
		struct Frequency
		{
			static const int id = 170;
		};
		struct Frequency_Value
		{
			static const int id = 171;
			typedef float Type;
		};
		struct Friction
		{
			static const int id = 184;
		};
		struct Friction_Value
		{
			static const int id = 185;
			typedef float Type;
		};
		struct Game
		{
			static const int id = 517;
		};
		struct GamePack
		{
			static const int id = 7;
		};
		struct GamePack_Description
		{
			static const int id = 8;
			typedef Menge::WString Type;
		};
		struct GamePack_Name
		{
			static const int id = 4;
			typedef Menge::ConstString Type;
		};
		struct GamePack_Path
		{
			static const int id = 6;
			typedef Menge::WString Type;
		};
		struct Glyph
		{
			static const int id = 511;
		};
		struct Glyph_Resource
		{
			static const int id = 512;
			typedef Menge::ConstString Type;
		};
		struct GlyphPath
		{
			static const int id = 293;
		};
		struct GlyphPath_Path
		{
			static const int id = 294;
			typedef Menge::WString Type;
		};
		struct Gravity2D
		{
			static const int id = 477;
		};
		struct Gravity2D_Value
		{
			static const int id = 478;
			typedef mt::vec2f Type;
		};
		struct GridSize
		{
			static const int id = 462;
		};
		struct GridSize_Value
		{
			static const int id = 463;
			typedef float Type;
		};
		struct GroupIndex
		{
			static const int id = 192;
		};
		struct GroupIndex_Value
		{
			static const int id = 193;
			typedef unsigned short Type;
		};
		struct HeadMode
		{
			static const int id = 216;
		};
		struct HeadMode_Value
		{
			static const int id = 217;
			typedef bool Type;
		};
		struct Height
		{
			static const int id = 210;
		};
		struct Height_Value
		{
			static const int id = 211;
			typedef float Type;
		};
		struct Hide
		{
			static const int id = 145;
		};
		struct Hide_Value
		{
			static const int id = 146;
			typedef bool Type;
		};
		struct HorizontalAlign
		{
			static const int id = 254;
		};
		struct HorizontalAlign_Value
		{
			static const int id = 255;
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
			static const int id = 295;
		};
		struct Image_Path
		{
			static const int id = 296;
			typedef Menge::WString Type;
		};
		struct ImageAlpha
		{
			static const int id = 218;
		};
		struct ImageAlpha_Name
		{
			static const int id = 219;
			typedef Menge::ConstString Type;
		};
		struct ImageIndex
		{
			static const int id = 164;
		};
		struct ImageIndex_Value
		{
			static const int id = 165;
			typedef size_t Type;
		};
		struct ImageMap
		{
			static const int id = 162;
		};
		struct ImageMap_Name
		{
			static const int id = 163;
			typedef Menge::ConstString Type;
		};
		struct ImageResource
		{
			static const int id = 311;
		};
		struct ImageResource_Name
		{
			static const int id = 312;
			typedef Menge::ConstString Type;
		};
		struct Indices
		{
			static const int id = 363;
		};
		struct Indices_Value
		{
			static const int id = 364;
			typedef std::vector<uint16> Type;
		};
		struct Internal
		{
			static const int id = 646;
		};
		struct Internal_Group
		{
			static const int id = 647;
			typedef Menge::ConstString Type;
		};
		struct Internal_Name
		{
			static const int id = 645;
			typedef Menge::ConstString Type;
		};
		struct IsBullet
		{
			static const int id = 200;
		};
		struct IsBullet_Value
		{
			static const int id = 201;
			typedef bool Type;
		};
		struct IsSensor
		{
			static const int id = 204;
		};
		struct IsSensor_Value
		{
			static const int id = 205;
			typedef bool Type;
		};
		struct IsStreamable
		{
			static const int id = 391;
		};
		struct IsStreamable_Value
		{
			static const int id = 392;
			typedef bool Type;
		};
		struct Kerning
		{
			static const int id = 309;
		};
		struct Kerning_advance
		{
			static const int id = 308;
			typedef float Type;
		};
		struct Kerning_id
		{
			static const int id = 310;
			typedef Menge::String Type;
		};
		struct KeyFrame2D
		{
			static const int id = 590;
		};
		struct KeyFrame2D_AnchorPoint
		{
			static const int id = 581;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Count
		{
			static const int id = 591;
			typedef size_t Type;
		};
		struct KeyFrame2D_Opacity
		{
			static const int id = 589;
			typedef float Type;
		};
		struct KeyFrame2D_Position
		{
			static const int id = 583;
			typedef mt::vec2f Type;
		};
		struct KeyFrame2D_Rotation
		{
			static const int id = 587;
			typedef float Type;
		};
		struct KeyFrame2D_Scale
		{
			static const int id = 585;
			typedef mt::vec2f Type;
		};
		struct KeyFrame3D
		{
			static const int id = 634;
		};
		struct KeyFrame3D_AnchorPoint
		{
			static const int id = 625;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Index
		{
			static const int id = 635;
			typedef size_t Type;
		};
		struct KeyFrame3D_Opacity
		{
			static const int id = 633;
			typedef float Type;
		};
		struct KeyFrame3D_Position
		{
			static const int id = 627;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Rotation
		{
			static const int id = 631;
			typedef mt::vec3f Type;
		};
		struct KeyFrame3D_Scale
		{
			static const int id = 629;
			typedef mt::vec3f Type;
		};
		struct KeyFrames2D
		{
			static const int id = 578;
		};
		struct KeyFrames2D_LayerIndex
		{
			static const int id = 579;
			typedef size_t Type;
		};
		struct KeyFrames3D
		{
			static const int id = 622;
		};
		struct KeyFrames3D_LayerIndex
		{
			static const int id = 623;
			typedef size_t Type;
		};
		struct KeyFramesPack
		{
			static const int id = 574;
		};
		struct KeyFramesPack_MaxIndex
		{
			static const int id = 575;
			typedef size_t Type;
		};
		struct KeyFramesPackPath
		{
			static const int id = 576;
		};
		struct KeyFramesPackPath_Path
		{
			static const int id = 577;
			typedef Menge::WString Type;
		};
		struct LanguagePack
		{
			static const int id = 69;
		};
		struct LanguagePack_Description
		{
			static const int id = 66;
			typedef Menge::WString Type;
		};
		struct LanguagePack_Locale
		{
			static const int id = 60;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Name
		{
			static const int id = 58;
			typedef Menge::ConstString Type;
		};
		struct LanguagePack_Path
		{
			static const int id = 64;
			typedef Menge::WString Type;
		};
		struct LanguagePack_Platform
		{
			static const int id = 62;
			typedef Menge::String Type;
		};
		struct LanguagePack_PreLoad
		{
			static const int id = 68;
			typedef bool Type;
		};
		struct LanguagePack_Type
		{
			static const int id = 70;
			typedef Menge::ConstString Type;
		};
		struct LineOffset
		{
			static const int id = 248;
		};
		struct LineOffset_Value
		{
			static const int id = 249;
			typedef float Type;
		};
		struct LinearDamping
		{
			static const int id = 194;
		};
		struct LinearDamping_Value
		{
			static const int id = 195;
			typedef float Type;
		};
		struct Loop
		{
			static const int id = 380;
		};
		struct Loop_Value
		{
			static const int id = 381;
			typedef bool Type;
		};
		struct Looped
		{
			static const int id = 176;
		};
		struct Looped_Value
		{
			static const int id = 177;
			typedef bool Type;
		};
		struct Looping
		{
			static const int id = 151;
		};
		struct Looping_Value
		{
			static const int id = 152;
			typedef bool Type;
		};
		struct Main
		{
			static const int id = 452;
		};
		struct Main_Value
		{
			static const int id = 453;
			typedef bool Type;
		};
		struct MaxWidth
		{
			static const int id = 244;
		};
		struct MaxWidth_Value
		{
			static const int id = 245;
			typedef float Type;
		};
		struct Model
		{
			static const int id = 353;
		};
		struct Model_Name
		{
			static const int id = 354;
			typedef Menge::ConstString Type;
		};
		struct Movie
		{
			static const int id = 554;
		};
		struct Movie_Name
		{
			static const int id = 555;
			typedef Menge::ConstString Type;
		};
		struct MovieCamera3D
		{
			static const int id = 620;
		};
		struct MovieCamera3D_CameraAspect
		{
			static const int id = 617;
			typedef float Type;
		};
		struct MovieCamera3D_CameraFOV
		{
			static const int id = 615;
			typedef float Type;
		};
		struct MovieCamera3D_CameraInterest
		{
			static const int id = 613;
			typedef mt::vec3f Type;
		};
		struct MovieCamera3D_CameraPosition
		{
			static const int id = 611;
			typedef mt::vec3f Type;
		};
		struct MovieCamera3D_Height
		{
			static const int id = 621;
			typedef float Type;
		};
		struct MovieCamera3D_Width
		{
			static const int id = 619;
			typedef float Type;
		};
		struct MovieLayer2D
		{
			static const int id = 572;
		};
		struct MovieLayer2D_BlendingMode
		{
			static const int id = 573;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_In
		{
			static const int id = 567;
			typedef float Type;
		};
		struct MovieLayer2D_Index
		{
			static const int id = 563;
			typedef size_t Type;
		};
		struct MovieLayer2D_Internal
		{
			static const int id = 565;
			typedef bool Type;
		};
		struct MovieLayer2D_Name
		{
			static const int id = 557;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_Out
		{
			static const int id = 569;
			typedef float Type;
		};
		struct MovieLayer2D_Parent
		{
			static const int id = 559;
			typedef size_t Type;
		};
		struct MovieLayer2D_Source
		{
			static const int id = 561;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer2D_StartInterval
		{
			static const int id = 571;
			typedef float Type;
		};
		struct MovieLayer3D
		{
			static const int id = 608;
		};
		struct MovieLayer3D_BlendingMode
		{
			static const int id = 609;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_In
		{
			static const int id = 603;
			typedef float Type;
		};
		struct MovieLayer3D_Index
		{
			static const int id = 599;
			typedef size_t Type;
		};
		struct MovieLayer3D_Internal
		{
			static const int id = 601;
			typedef bool Type;
		};
		struct MovieLayer3D_Name
		{
			static const int id = 593;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_Out
		{
			static const int id = 605;
			typedef float Type;
		};
		struct MovieLayer3D_Parent
		{
			static const int id = 595;
			typedef size_t Type;
		};
		struct MovieLayer3D_Source
		{
			static const int id = 597;
			typedef Menge::ConstString Type;
		};
		struct MovieLayer3D_StartInterval
		{
			static const int id = 607;
			typedef float Type;
		};
		struct Node
		{
			static const int id = 141;
		};
		struct Node_Name
		{
			static const int id = 138;
			typedef Menge::ConstString Type;
		};
		struct Node_Tag
		{
			static const int id = 142;
			typedef Menge::ConstString Type;
		};
		struct Node_Type
		{
			static const int id = 140;
			typedef Menge::ConstString Type;
		};
		struct Outline
		{
			static const int id = 242;
		};
		struct Outline_Value
		{
			static const int id = 243;
			typedef bool Type;
		};
		struct OutlineColor
		{
			static const int id = 240;
		};
		struct OutlineColor_Value
		{
			static const int id = 241;
			typedef Menge::ColourValue Type;
		};
		struct OutlineImage
		{
			static const int id = 297;
		};
		struct OutlineImage_Path
		{
			static const int id = 298;
			typedef Menge::WString Type;
		};
		struct Pak
		{
			static const int id = 519;
		};
		struct Parallax
		{
			static const int id = 456;
		};
		struct Parallax_Factor
		{
			static const int id = 457;
			typedef mt::vec2f Type;
		};
		struct Param
		{
			static const int id = 127;
		};
		struct Param_Name
		{
			static const int id = 94;
			typedef Menge::ConstString Type;
		};
		struct Param_Value
		{
			static const int id = 96;
			typedef Menge::String Type;
		};
		struct Param_Value1
		{
			static const int id = 98;
			typedef Menge::String Type;
		};
		struct Param_Value10
		{
			static const int id = 116;
			typedef Menge::String Type;
		};
		struct Param_Value11
		{
			static const int id = 118;
			typedef Menge::String Type;
		};
		struct Param_Value12
		{
			static const int id = 120;
			typedef Menge::String Type;
		};
		struct Param_Value13
		{
			static const int id = 122;
			typedef Menge::String Type;
		};
		struct Param_Value14
		{
			static const int id = 124;
			typedef Menge::String Type;
		};
		struct Param_Value15
		{
			static const int id = 126;
			typedef Menge::String Type;
		};
		struct Param_Value16
		{
			static const int id = 128;
			typedef Menge::String Type;
		};
		struct Param_Value2
		{
			static const int id = 100;
			typedef Menge::String Type;
		};
		struct Param_Value3
		{
			static const int id = 102;
			typedef Menge::String Type;
		};
		struct Param_Value4
		{
			static const int id = 104;
			typedef Menge::String Type;
		};
		struct Param_Value5
		{
			static const int id = 106;
			typedef Menge::String Type;
		};
		struct Param_Value6
		{
			static const int id = 108;
			typedef Menge::String Type;
		};
		struct Param_Value7
		{
			static const int id = 110;
			typedef Menge::String Type;
		};
		struct Param_Value8
		{
			static const int id = 112;
			typedef Menge::String Type;
		};
		struct Param_Value9
		{
			static const int id = 114;
			typedef Menge::String Type;
		};
		struct Params
		{
			static const int id = 91;
		};
		struct Params_Path
		{
			static const int id = 92;
			typedef Menge::String Type;
		};
		struct PercentVisibility
		{
			static const int id = 222;
		};
		struct PercentVisibility_Value
		{
			static const int id = 223;
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
			static const int id = 481;
		};
		struct PhysicCanSleep_Value
		{
			static const int id = 482;
			typedef bool Type;
		};
		struct PhysicWorld2DBox
		{
			static const int id = 479;
		};
		struct PhysicWorld2DBox_Value
		{
			static const int id = 480;
			typedef mt::vec4f Type;
		};
		struct PlayerCounter
		{
			static const int id = 133;
		};
		struct PlayerCounter_Value
		{
			static const int id = 134;
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
			static const int id = 160;
		};
		struct Point_Value
		{
			static const int id = 161;
			typedef mt::vec2f Type;
		};
		struct Polygon
		{
			static const int id = 159;
		};
		struct Position
		{
			static const int id = 208;
		};
		struct Position_Value
		{
			static const int id = 209;
			typedef mt::vec2f Type;
		};
		struct PositionOffset
		{
			static const int id = 468;
		};
		struct PositionOffset_Value
		{
			static const int id = 469;
			typedef mt::vec2f Type;
		};
		struct Project
		{
			static const int id = 71;
		};
		struct Project_Name
		{
			static const int id = 72;
			typedef Menge::ConstString Type;
		};
		struct Radius
		{
			static const int id = 206;
		};
		struct Radius_Value
		{
			static const int id = 207;
			typedef float Type;
		};
		struct RandomStart
		{
			static const int id = 155;
		};
		struct RandomStart_Value
		{
			static const int id = 156;
			typedef bool Type;
		};
		struct RenderArea
		{
			static const int id = 458;
		};
		struct RenderArea_Value
		{
			static const int id = 459;
			typedef Menge::Viewport Type;
		};
		struct RenderTarget
		{
			static const int id = 485;
		};
		struct RenderTarget_Name
		{
			static const int id = 484;
			typedef Menge::ConstString Type;
		};
		struct RenderTarget_Size
		{
			static const int id = 486;
			typedef mt::vec2f Type;
		};
		struct RenderTargetName
		{
			static const int id = 464;
		};
		struct RenderTargetName_Value
		{
			static const int id = 465;
			typedef Menge::ConstString Type;
		};
		struct RenderViewport
		{
			static const int id = 460;
		};
		struct RenderViewport_Value
		{
			static const int id = 461;
			typedef Menge::Viewport Type;
		};
		struct Resource
		{
			static const int id = 450;
		};
		struct Resource_Name
		{
			static const int id = 449;
			typedef Menge::ConstString Type;
		};
		struct Resource_Path
		{
			static const int id = 78;
			typedef Menge::WString Type;
		};
		struct Resource_Type
		{
			static const int id = 451;
			typedef Menge::ConstString Type;
		};
		struct Resource_Value
		{
			static const int id = 259;
			typedef Menge::ConstString Type;
		};
		struct ResourceAtlas
		{
			static const int id = 283;
		};
		struct ResourceAtlas_Name
		{
			static const int id = 284;
			typedef Menge::ConstString Type;
		};
		struct ResourceGlyph
		{
			static const int id = 291;
		};
		struct ResourceGlyph_Name
		{
			static const int id = 292;
			typedef Menge::ConstString Type;
		};
		struct ResourceImage
		{
			static const int id = 262;
		};
		struct ResourceImage_Name
		{
			static const int id = 263;
			typedef Menge::ConstString Type;
		};
		struct ResourcePack
		{
			static const int id = 55;
		};
		struct ResourcePack_Description
		{
			static const int id = 54;
			typedef Menge::WString Type;
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
			static const int id = 52;
			typedef Menge::WString Type;
		};
		struct ResourcePack_PreLoad
		{
			static const int id = 56;
			typedef bool Type;
		};
		struct ResourcePack_Type
		{
			static const int id = 50;
			typedef Menge::ConstString Type;
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
			static const int id = 264;
		};
		struct ResourceSequence_Name
		{
			static const int id = 265;
			typedef Menge::ConstString Type;
		};
		struct ResourceSound
		{
			static const int id = 268;
		};
		struct ResourceSound_Name
		{
			static const int id = 269;
			typedef Menge::ConstString Type;
		};
		struct ResourceTileSet
		{
			static const int id = 393;
		};
		struct ResourceTileSet_Name
		{
			static const int id = 394;
			typedef Menge::String Type;
		};
		struct ResourceVideo
		{
			static const int id = 266;
		};
		struct ResourceVideo_Name
		{
			static const int id = 267;
			typedef Menge::ConstString Type;
		};
		struct Resources
		{
			static const int id = 520;
		};
		struct Resources_Path
		{
			static const int id = 521;
			typedef Menge::WString Type;
		};
		struct Restitution
		{
			static const int id = 186;
		};
		struct Restitution_Value
		{
			static const int id = 187;
			typedef float Type;
		};
		struct RightAlign
		{
			static const int id = 238;
		};
		struct RightAlign_Value
		{
			static const int id = 239;
			typedef bool Type;
		};
		struct Scale
		{
			static const int id = 546;
		};
		struct Scale_Value
		{
			static const int id = 547;
			typedef mt::vec2f Type;
		};
		struct Scene
		{
			static const int id = 475;
		};
		struct Scene_Name
		{
			static const int id = 472;
			typedef Menge::ConstString Type;
		};
		struct Scene_Prototype
		{
			static const int id = 476;
			typedef Menge::ConstString Type;
		};
		struct Scene_Script
		{
			static const int id = 474;
			typedef bool Type;
		};
		struct Scenes
		{
			static const int id = 73;
		};
		struct Scenes_Path
		{
			static const int id = 74;
			typedef Menge::WString Type;
		};
		struct Screensaver
		{
			static const int id = 548;
		};
		struct Screensaver_Name
		{
			static const int id = 549;
			typedef Menge::WString Type;
		};
		struct Scripts
		{
			static const int id = 79;
		};
		struct Scripts_Path
		{
			static const int id = 80;
			typedef Menge::WString Type;
		};
		struct Sequence
		{
			static const int id = 281;
		};
		struct Sequence_Delay
		{
			static const int id = 280;
			typedef float Type;
		};
		struct Sequence_Index
		{
			static const int id = 278;
			typedef size_t Type;
		};
		struct Sequence_ResourceImageName
		{
			static const int id = 282;
			typedef Menge::ConstString Type;
		};
		struct SequenceArray
		{
			static const int id = 287;
		};
		struct SequenceArray_Count
		{
			static const int id = 286;
			typedef size_t Type;
		};
		struct SequenceArray_Delay
		{
			static const int id = 288;
			typedef float Type;
		};
		struct Sequences
		{
			static const int id = 276;
		};
		struct Shape
		{
			static const int id = 513;
		};
		struct ShapeBox
		{
			static const int id = 515;
		};
		struct ShapeCircle
		{
			static const int id = 514;
		};
		struct Shuffle
		{
			static const int id = 382;
		};
		struct Shuffle_Value
		{
			static const int id = 383;
			typedef bool Type;
		};
		struct Size
		{
			static const int id = 454;
		};
		struct Size_Value
		{
			static const int id = 455;
			typedef mt::vec2f Type;
		};
		struct StartPosition
		{
			static const int id = 178;
		};
		struct StartPosition_Value
		{
			static const int id = 179;
			typedef float Type;
		};
		struct SubScene
		{
			static const int id = 552;
		};
		struct SubScene_Name
		{
			static const int id = 553;
			typedef Menge::ConstString Type;
		};
		struct Text
		{
			static const int id = 497;
		};
		struct Text_CharOffset
		{
			static const int id = 494;
			typedef float Type;
		};
		struct Text_Demand
		{
			static const int id = 90;
			typedef bool Type;
		};
		struct Text_File
		{
			static const int id = 88;
			typedef Menge::WString Type;
		};
		struct Text_Font
		{
			static const int id = 498;
			typedef Menge::ConstString Type;
		};
		struct Text_Key
		{
			static const int id = 488;
			typedef Menge::ConstString Type;
		};
		struct Text_LineOffset
		{
			static const int id = 496;
			typedef float Type;
		};
		struct Text_Name
		{
			static const int id = 86;
			typedef Menge::ConstString Type;
		};
		struct Text_Value
		{
			static const int id = 490;
			typedef Menge::WString Type;
		};
		struct TextKey
		{
			static const int id = 491;
		};
		struct TextKey_Value
		{
			static const int id = 492;
			typedef Menge::ConstString Type;
		};
		struct Texts
		{
			static const int id = 503;
		};
		struct Texts_CharOffset
		{
			static const int id = 502;
			typedef float Type;
		};
		struct Texts_Font
		{
			static const int id = 500;
			typedef Menge::ConstString Type;
		};
		struct Texts_LineOffset
		{
			static const int id = 504;
			typedef float Type;
		};
		struct Texts_Path
		{
			static const int id = 84;
			typedef Menge::WString Type;
		};
		struct Texture
		{
			static const int id = 351;
		};
		struct Texture_Color
		{
			static const int id = 352;
			typedef Menge::ColourValue Type;
		};
		struct Texture_Name
		{
			static const int id = 350;
			typedef Menge::String Type;
		};
		struct Tile
		{
			static const int id = 409;
		};
		struct Tile_Image
		{
			static const int id = 406;
			typedef Menge::ConstString Type;
		};
		struct Tile_ImageBack
		{
			static const int id = 408;
			typedef Menge::ConstString Type;
		};
		struct Tile_JuncImage
		{
			static const int id = 410;
			typedef Menge::ConstString Type;
		};
		struct Tile_MaxAngle
		{
			static const int id = 404;
			typedef float Type;
		};
		struct Tile_MinAngle
		{
			static const int id = 402;
			typedef float Type;
		};
		struct TileJunc
		{
			static const int id = 411;
		};
		struct TileJunc_Image
		{
			static const int id = 412;
			typedef Menge::ConstString Type;
		};
		struct TileMap
		{
			static const int id = 395;
		};
		struct TileMap_File
		{
			static const int id = 396;
			typedef Menge::String Type;
		};
		struct TileMap_Name
		{
			static const int id = 257;
			typedef Menge::String Type;
		};
		struct TileSet
		{
			static const int id = 399;
		};
		struct TileSet_File
		{
			static const int id = 400;
			typedef Menge::String Type;
		};
		struct Tiles
		{
			static const int id = 397;
		};
		struct Tiles_Value
		{
			static const int id = 398;
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
			typedef Menge::WString Type;
		};
		struct Track
		{
			static const int id = 389;
		};
		struct Track_Codec
		{
			static const int id = 388;
			typedef Menge::ConstString Type;
		};
		struct Track_File
		{
			static const int id = 386;
			typedef Menge::WString Type;
		};
		struct Track_Volume
		{
			static const int id = 390;
			typedef float Type;
		};
		struct Tracks
		{
			static const int id = 384;
		};
		struct Transformation
		{
			static const int id = 544;
		};
		struct Transformation_Angle
		{
			static const int id = 541;
			typedef float Type;
		};
		struct Transformation_Direction
		{
			static const int id = 533;
			typedef mt::vec2f Type;
		};
		struct Transformation_FixedRotation
		{
			static const int id = 545;
			typedef bool Type;
		};
		struct Transformation_Origin
		{
			static const int id = 543;
			typedef mt::vec2f Type;
		};
		struct Transformation_Position
		{
			static const int id = 531;
			typedef mt::vec2f Type;
		};
		struct Transformation_Rotate
		{
			static const int id = 539;
			typedef float Type;
		};
		struct Transformation_Scale
		{
			static const int id = 537;
			typedef mt::vec2f Type;
		};
		struct Transformation_Value
		{
			static const int id = 535;
			typedef mt::mat3f Type;
		};
		struct Vertex
		{
			static const int id = 370;
		};
		struct Vertex_Coordinate
		{
			static const int id = 369;
			typedef mt::vec2f Type;
		};
		struct Vertex_UV
		{
			static const int id = 371;
			typedef mt::vec2f Type;
		};
		struct Vertex_Value
		{
			static const int id = 215;
			typedef mt::vec2f Type;
		};
		struct VerticalAlign
		{
			static const int id = 252;
		};
		struct VerticalAlign_Value
		{
			static const int id = 253;
			typedef int Type;
		};
		struct Vertices
		{
			static const int id = 365;
		};
		struct Vertices_Size
		{
			static const int id = 366;
			typedef size_t Type;
		};
		struct ViewportSize
		{
			static const int id = 466;
		};
		struct ViewportSize_Value
		{
			static const int id = 467;
			typedef mt::vec2f Type;
		};
		struct Width
		{
			static const int id = 372;
		};
		struct Width_Value
		{
			static const int id = 373;
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
		struct WindowBackground
		{
			static const int id = 413;
		};
		struct WindowBackground_ResourceImageName
		{
			static const int id = 414;
			typedef Menge::ConstString Type;
		};
		struct WindowBottom
		{
			static const int id = 437;
		};
		struct WindowBottom_Offset
		{
			static const int id = 438;
			typedef mt::vec2f Type;
		};
		struct WindowBottom_ResourceImageName
		{
			static const int id = 436;
			typedef Menge::ConstString Type;
		};
		struct WindowLeft
		{
			static const int id = 445;
		};
		struct WindowLeft_Offset
		{
			static const int id = 446;
			typedef mt::vec2f Type;
		};
		struct WindowLeft_ResourceImageName
		{
			static const int id = 444;
			typedef Menge::ConstString Type;
		};
		struct WindowLeftBottom
		{
			static const int id = 441;
		};
		struct WindowLeftBottom_Offset
		{
			static const int id = 442;
			typedef mt::vec2f Type;
		};
		struct WindowLeftBottom_ResourceImageName
		{
			static const int id = 440;
			typedef Menge::ConstString Type;
		};
		struct WindowLeftTop
		{
			static const int id = 417;
		};
		struct WindowLeftTop_Offset
		{
			static const int id = 418;
			typedef mt::vec2f Type;
		};
		struct WindowLeftTop_ResourceImageName
		{
			static const int id = 416;
			typedef Menge::ConstString Type;
		};
		struct WindowResource
		{
			static const int id = 270;
		};
		struct WindowResource_Name
		{
			static const int id = 271;
			typedef Menge::ConstString Type;
		};
		struct WindowRight
		{
			static const int id = 429;
		};
		struct WindowRight_Offset
		{
			static const int id = 430;
			typedef mt::vec2f Type;
		};
		struct WindowRight_ResourceImageName
		{
			static const int id = 428;
			typedef Menge::ConstString Type;
		};
		struct WindowRightBottom
		{
			static const int id = 433;
		};
		struct WindowRightBottom_Offset
		{
			static const int id = 434;
			typedef mt::vec2f Type;
		};
		struct WindowRightBottom_ResourceImageName
		{
			static const int id = 432;
			typedef Menge::ConstString Type;
		};
		struct WindowRightTop
		{
			static const int id = 425;
		};
		struct WindowRightTop_Offset
		{
			static const int id = 426;
			typedef mt::vec2f Type;
		};
		struct WindowRightTop_ResourceImageName
		{
			static const int id = 424;
			typedef Menge::ConstString Type;
		};
		struct WindowTop
		{
			static const int id = 421;
		};
		struct WindowTop_Offset
		{
			static const int id = 422;
			typedef mt::vec2f Type;
		};
		struct WindowTop_ResourceImageName
		{
			static const int id = 420;
			typedef Menge::ConstString Type;
		};
		struct WorkAreaDuration
		{
			static const int id = 642;
		};
		struct WorkAreaDuration_Value
		{
			static const int id = 643;
			typedef float Type;
		};
	}
}
