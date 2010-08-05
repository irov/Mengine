#pragma once
namespace Menge
{
    namespace Protocol
    {
        struct BaseDir
        {
            static const int id = 1;
        };
        struct GamePack
        {
            static const int id = 3;
        };
        struct AlreadyRunningPolicy
        {
            static const int id = 7;
        };
        struct AllowFullscreenSwitchShortcut
        {
            static const int id = 9;
        };
        struct Title
        {
            static const int id = 11;
        };
        struct ResourceResolution
        {
            static const int id = 14;
        };
        struct ContentResolution
        {
            static const int id = 16;
        };
        struct FixedContentResolution
        {
            static const int id = 18;
        };
        struct PersonalityModule
        {
            static const int id = 20;
        };
        struct Icon
        {
            static const int id = 22;
        };
        struct DefaultArrow
        {
            static const int id = 24;
        };
        struct Window
        {
            static const int id = 26;
        };
        struct ResourcePack
        {
            static const int id = 33;
        };
        struct LanguagePack
        {
            static const int id = 38;
        };
        struct Scenes
        {
            static const int id = 43;
        };
        struct Entities
        {
            static const int id = 45;
        };
        struct Resource
        {
            static const int id = 47;
        };
        struct Scripts
        {
            static const int id = 49;
        };
        struct Arrows
        {
            static const int id = 51;
        };
        struct Text
        {
            static const int id = 53;
        };
        struct AccountID
        {
            static const int id = 55;
        };
        struct DefaultAccountID
        {
            static const int id = 57;
        };
        struct PlayerCounter
        {
            static const int id = 59;
        };
        struct Node
        {
            static const int id = 61;
        };
        struct Enable
        {
            static const int id = 64;
        };
        struct Hide
        {
            static const int id = 66;
        };
        struct Color
        {
            static const int id = 68;
        };
        struct Animation
        {
            static const int id = 70;
        };
        struct Looping
        {
            static const int id = 72;
        };
        struct AutoStart
        {
            static const int id = 74;
        };
        struct RandomStart
        {
            static const int id = 76;
        };
        struct ClickOffset
        {
            static const int id = 78;
        };
        struct Polygon
        {
            static const int id = 80;
        };
        struct Point
        {
            static const int id = 82;
        };
        struct ImageMap
        {
            static const int id = 84;
        };
        struct ImageIndex
        {
            static const int id = 86;
        };
        struct AlphaTest
        {
            static const int id = 88;
        };
        struct Amplitude
        {
            static const int id = 90;
        };
        struct Frequency
        {
            static const int id = 92;
        };
        struct Emitter
        {
            static const int id = 94;
        };
        struct AutoPlay
        {
            static const int id = 96;
        };
        struct Looped
        {
            static const int id = 98;
        };
        struct StartPosition
        {
            static const int id = 100;
        };
        struct EmitterRelative
        {
            static const int id = 102;
        };
        struct Density
        {
            static const int id = 104;
        };
        struct Friction
        {
            static const int id = 106;
        };
        struct Restitution
        {
            static const int id = 108;
        };
        struct CollisionMask
        {
            static const int id = 110;
        };
        struct CategoryBits
        {
            static const int id = 112;
        };
        struct GroupIndex
        {
            static const int id = 114;
        };
        struct LinearDamping
        {
            static const int id = 116;
        };
        struct AngularDamping
        {
            static const int id = 118;
        };
        struct AllowSleep
        {
            static const int id = 120;
        };
        struct IsBullet
        {
            static const int id = 122;
        };
        struct FixedRotation
        {
            static const int id = 124;
        };
        struct IsSensor
        {
            static const int id = 126;
        };
        struct Radius
        {
            static const int id = 128;
        };
        struct Position
        {
            static const int id = 130;
        };
        struct Height
        {
            static const int id = 132;
        };
        struct Angle
        {
            static const int id = 134;
        };
        struct Vertex
        {
            static const int id = 136;
        };
        struct HeadMode
        {
            static const int id = 138;
        };
        struct ImageAlpha
        {
            static const int id = 140;
        };
        struct CenterAlign
        {
            static const int id = 142;
        };
        struct PercentVisibility
        {
            static const int id = 144;
        };
        struct BlendSource
        {
            static const int id = 146;
        };
        struct BlendDest
        {
            static const int id = 148;
        };
        struct Font
        {
            static const int id = 150;
        };
        struct RightAlign
        {
            static const int id = 152;
        };
        struct OutlineColor
        {
            static const int id = 154;
        };
        struct Outline
        {
            static const int id = 156;
        };
        struct MaxWidth
        {
            static const int id = 158;
        };
        struct CharOffset
        {
            static const int id = 160;
        };
        struct LineOffset
        {
            static const int id = 162;
        };
        struct FieldSize
        {
            static const int id = 164;
        };
        struct VerticalAlign
        {
            static const int id = 166;
        };
        struct HorizontalAlign
        {
            static const int id = 168;
        };
        struct TileMap
        {
            static const int id = 170;
        };
        struct Edge
        {
            static const int id = 173;
        };
        struct ResourceVideo
        {
            static const int id = 175;
        };
        struct ResourceSound
        {
            static const int id = 177;
        };
        struct WindowResource
        {
            static const int id = 179;
        };
        struct ClientSize
        {
            static const int id = 181;
        };
        struct ClientSizeInTiles
        {
            static const int id = 183;
        };
        struct Sequences
        {
            static const int id = 185;
        };
        struct Sequence
        {
            static const int id = 186;
        };
        struct SequenceArray
        {
            static const int id = 189;
        };
        struct Folder
        {
            static const int id = 192;
        };
        struct Fontdef
        {
            static const int id = 194;
        };
        struct Image
        {
            static const int id = 196;
        };
        struct OutlineImage
        {
            static const int id = 198;
        };
        struct Char
        {
            static const int id = 201;
        };
        struct ImageResource
        {
            static const int id = 206;
        };
        struct Filter
        {
            static const int id = 208;
        };
        struct File
        {
            static const int id = 210;
        };
        struct Texture
        {
            static const int id = 221;
        };
        struct Mesh
        {
            static const int id = 224;
        };
        struct Width
        {
            static const int id = 226;
        };
        struct Depth
        {
            static const int id = 228;
        };
        struct ConcaveGeometryFile
        {
            static const int id = 230;
        };
        struct ConvexGeometryFile
        {
            static const int id = 232;
        };
        struct Loop
        {
            static const int id = 234;
        };
        struct Shuffle
        {
            static const int id = 236;
        };
        struct Tracks
        {
            static const int id = 238;
        };
        struct Track
        {
            static const int id = 239;
        };
        struct IsStreamable
        {
            static const int id = 242;
        };
        struct ResourceTileSet
        {
            static const int id = 244;
        };
        struct Tiles
        {
            static const int id = 247;
        };
        struct TileSet
        {
            static const int id = 249;
        };
        struct Tile
        {
            static const int id = 251;
        };
        struct TileJunc
        {
            static const int id = 257;
        };
        struct Background
        {
            static const int id = 259;
        };
        struct LeftTop
        {
            static const int id = 262;
        };
        struct Top
        {
            static const int id = 265;
        };
        struct RightTop
        {
            static const int id = 268;
        };
        struct Right
        {
            static const int id = 271;
        };
        struct RightBottom
        {
            static const int id = 274;
        };
        struct Bottom
        {
            static const int id = 277;
        };
        struct LeftBottom
        {
            static const int id = 280;
        };
        struct Left
        {
            static const int id = 283;
        };
        struct DataBlock
        {
            static const int id = 286;
        };
        struct Main
        {
            static const int id = 289;
        };
        struct Size
        {
            static const int id = 291;
        };
        struct Parallax
        {
            static const int id = 293;
        };
        struct RenderArea
        {
            static const int id = 295;
        };
        struct RenderViewport
        {
            static const int id = 297;
        };
        struct GridSize
        {
            static const int id = 299;
        };
        struct RenderTargetName
        {
            static const int id = 301;
        };
        struct ViewportSize
        {
            static const int id = 303;
        };
        struct Scene
        {
            static const int id = 305;
        };
        struct Gravity2D
        {
            static const int id = 307;
        };
        struct PhysicWorld2DBox
        {
            static const int id = 309;
        };
        struct PhysicCanSleep
        {
            static const int id = 311;
        };
        struct RenderTarget
        {
            static const int id = 313;
        };
        struct Texts
        {
            static const int id = 321;
        };
        struct Frame
        {
            static const int id = 327;
        };
        struct Cell
        {
            static const int id = 329;
        };
        struct Glyph
        {
            static const int id = 332;
        };
        struct Shape
        {
            static const int id = 334;
        };
        struct ShapeCircle
        {
            static const int id = 335;
        };
        struct ShapeBox
        {
            static const int id = 336;
        };
        struct Application
        {
            static const int id = 337;
        };
        struct Game
        {
            static const int id = 338;
        };
        struct Default
        {
            static const int id = 339;
        };
        struct Pak
        {
            static const int id = 340;
        };
        struct Resources
        {
            static const int id = 341;
        };
        struct Arrow
        {
            static const int id = 343;
        };
        struct Entity
        {
            static const int id = 345;
        };
        struct Transformation
        {
            static const int id = 348;
        };
        struct Scale
        {
            static const int id = 357;
        };
        struct Screensaver
        {
            static const int id = 359;
        };
        struct Alpha
        {
            static const int id = 361;
        };
        struct BaseDir_Value
        {
            static const int id = 2;
            typedef Menge::String Type;
        };
        struct GamePack_Name
        {
            static const int id = 4;
            typedef Menge::ConstString Type;
        };
        struct GamePack_Path
        {
            static const int id = 5;
            typedef Menge::String Type;
        };
        struct GamePack_Description
        {
            static const int id = 6;
            typedef Menge::String Type;
        };
        struct AlreadyRunningPolicy_Value
        {
            static const int id = 8;
            typedef int Type;
        };
        struct AllowFullscreenSwitchShortcut_Value
        {
            static const int id = 10;
            typedef bool Type;
        };
        struct Title_Value
        {
            static const int id = 12;
            typedef Menge::ConstString Type;
        };
        struct Title_Localized
        {
            static const int id = 13;
            typedef bool Type;
        };
        struct ResourceResolution_Value
        {
            static const int id = 15;
            typedef Menge::Resolution Type;
        };
        struct ContentResolution_Value
        {
            static const int id = 17;
            typedef Menge::Resolution Type;
        };
        struct FixedContentResolution_Value
        {
            static const int id = 19;
            typedef bool Type;
        };
        struct PersonalityModule_Value
        {
            static const int id = 21;
            typedef Menge::ConstString Type;
        };
        struct Icon_Path
        {
            static const int id = 23;
            typedef Menge::String Type;
        };
        struct DefaultArrow_Value
        {
            static const int id = 25;
            typedef Menge::ConstString Type;
        };
        struct Window_Size
        {
            static const int id = 27;
            typedef Menge::Resolution Type;
        };
        struct Window_Bits
        {
            static const int id = 28;
            typedef int Type;
        };
        struct Window_Fullscreen
        {
            static const int id = 29;
            typedef bool Type;
        };
        struct Window_HasPanel
        {
            static const int id = 30;
            typedef bool Type;
        };
        struct Window_VSync
        {
            static const int id = 31;
            typedef bool Type;
        };
        struct Window_TextureFiltering
        {
            static const int id = 32;
            typedef bool Type;
        };
        struct ResourcePack_Name
        {
            static const int id = 34;
            typedef Menge::ConstString Type;
        };
        struct ResourcePack_Path
        {
            static const int id = 35;
            typedef Menge::String Type;
        };
        struct ResourcePack_Description
        {
            static const int id = 36;
            typedef Menge::String Type;
        };
        struct ResourcePack_PreLoad
        {
            static const int id = 37;
            typedef bool Type;
        };
        struct LanguagePack_Name
        {
            static const int id = 39;
            typedef Menge::ConstString Type;
        };
        struct LanguagePack_Path
        {
            static const int id = 40;
            typedef Menge::String Type;
        };
        struct LanguagePack_Description
        {
            static const int id = 41;
            typedef Menge::String Type;
        };
        struct LanguagePack_PreLoad
        {
            static const int id = 42;
            typedef bool Type;
        };
        struct Scenes_Path
        {
            static const int id = 44;
            typedef Menge::String Type;
        };
        struct Entities_Path
        {
            static const int id = 46;
            typedef Menge::String Type;
        };
        struct Resource_Path
        {
            static const int id = 48;
            typedef Menge::String Type;
        };
        struct Scripts_Path
        {
            static const int id = 50;
            typedef Menge::String Type;
        };
        struct Arrows_Path
        {
            static const int id = 52;
            typedef Menge::String Type;
        };
        struct Text_Path
        {
            static const int id = 54;
            typedef Menge::String Type;
        };
        struct AccountID_Value
        {
            static const int id = 56;
            typedef Menge::String Type;
        };
        struct DefaultAccountID_Value
        {
            static const int id = 58;
            typedef Menge::String Type;
        };
        struct PlayerCounter_Value
        {
            static const int id = 60;
            typedef int Type;
        };
        struct Node_Name
        {
            static const int id = 62;
            typedef Menge::ConstString Type;
        };
        struct Node_Type
        {
            static const int id = 63;
            typedef Menge::ConstString Type;
        };
        struct Enable_Value
        {
            static const int id = 65;
            typedef bool Type;
        };
        struct Hide_Value
        {
            static const int id = 67;
            typedef bool Type;
        };
        struct Color_Value
        {
            static const int id = 69;
            typedef Menge::ColourValue Type;
        };
        struct Animation_Name
        {
            static const int id = 71;
            typedef Menge::ConstString Type;
        };
        struct Looping_Value
        {
            static const int id = 73;
            typedef bool Type;
        };
        struct AutoStart_Value
        {
            static const int id = 75;
            typedef bool Type;
        };
        struct RandomStart_Value
        {
            static const int id = 77;
            typedef bool Type;
        };
        struct ClickOffset_Value
        {
            static const int id = 79;
            typedef mt::vec2f Type;
        };
        struct Polygon_Point
        {
            static const int id = 81;
            typedef mt::vec2f Type;
        };
        struct Point_Value
        {
            static const int id = 83;
            typedef mt::vec2f Type;
        };
        struct ImageMap_Name
        {
            static const int id = 85;
            typedef Menge::ConstString Type;
        };
        struct ImageIndex_Value
        {
            static const int id = 87;
            typedef size_t Type;
        };
        struct AlphaTest_Value
        {
            static const int id = 89;
            typedef float Type;
        };
        struct Amplitude_Value
        {
            static const int id = 91;
            typedef float Type;
        };
        struct Frequency_Value
        {
            static const int id = 93;
            typedef float Type;
        };
        struct Emitter_Name
        {
            static const int id = 95;
            typedef Menge::ConstString Type;
        };
        struct AutoPlay_Value
        {
            static const int id = 97;
            typedef bool Type;
        };
        struct Looped_Value
        {
            static const int id = 99;
            typedef bool Type;
        };
        struct StartPosition_Value
        {
            static const int id = 101;
            typedef float Type;
        };
        struct EmitterRelative_Value
        {
            static const int id = 103;
            typedef bool Type;
        };
        struct Density_Value
        {
            static const int id = 105;
            typedef float Type;
        };
        struct Friction_Value
        {
            static const int id = 107;
            typedef float Type;
        };
        struct Restitution_Value
        {
            static const int id = 109;
            typedef float Type;
        };
        struct CollisionMask_Value
        {
            static const int id = 111;
            typedef size_t Type;
        };
        struct CategoryBits_Value
        {
            static const int id = 113;
            typedef size_t Type;
        };
        struct GroupIndex_Value
        {
            static const int id = 115;
            typedef unsigned short Type;
        };
        struct LinearDamping_Value
        {
            static const int id = 117;
            typedef float Type;
        };
        struct AngularDamping_Value
        {
            static const int id = 119;
            typedef float Type;
        };
        struct AllowSleep_Value
        {
            static const int id = 121;
            typedef bool Type;
        };
        struct IsBullet_Value
        {
            static const int id = 123;
            typedef bool Type;
        };
        struct FixedRotation_Value
        {
            static const int id = 125;
            typedef bool Type;
        };
        struct IsSensor_Value
        {
            static const int id = 127;
            typedef bool Type;
        };
        struct Radius_Value
        {
            static const int id = 129;
            typedef float Type;
        };
        struct Position_Value
        {
            static const int id = 131;
            typedef mt::vec2f Type;
        };
        struct Height_Value
        {
            static const int id = 133;
            typedef float Type;
        };
        struct Angle_Value
        {
            static const int id = 135;
            typedef float Type;
        };
        struct Vertex_Value
        {
            static const int id = 137;
            typedef mt::vec2f Type;
        };
        struct HeadMode_Value
        {
            static const int id = 139;
            typedef bool Type;
        };
        struct ImageAlpha_Name
        {
            static const int id = 141;
            typedef Menge::ConstString Type;
        };
        struct CenterAlign_Value
        {
            static const int id = 143;
            typedef bool Type;
        };
        struct PercentVisibility_Value
        {
            static const int id = 145;
            typedef mt::vec4f Type;
        };
        struct BlendSource_Value
        {
            static const int id = 147;
            typedef int Type;
        };
        struct BlendDest_Value
        {
            static const int id = 149;
            typedef int Type;
        };
        struct Font_Name
        {
            static const int id = 151;
            typedef Menge::ConstString Type;
        };
        struct RightAlign_Value
        {
            static const int id = 153;
            typedef bool Type;
        };
        struct OutlineColor_Value
        {
            static const int id = 155;
            typedef Menge::ColourValue Type;
        };
        struct Outline_Value
        {
            static const int id = 157;
            typedef bool Type;
        };
        struct MaxWidth_Value
        {
            static const int id = 159;
            typedef float Type;
        };
        struct CharOffset_Value
        {
            static const int id = 161;
            typedef float Type;
        };
        struct LineOffset_Value
        {
            static const int id = 163;
            typedef float Type;
        };
        struct FieldSize_Value
        {
            static const int id = 165;
            typedef mt::vec2f Type;
        };
        struct VerticalAlign_Value
        {
            static const int id = 167;
            typedef int Type;
        };
        struct HorizontalAlign_Value
        {
            static const int id = 169;
            typedef int Type;
        };
        struct TileMap_Name
        {
            static const int id = 171;
            typedef Menge::String Type;
        };
        struct Resource_Value
        {
            static const int id = 172;
            typedef Menge::ConstString Type;
        };
        struct Edge_Layer
        {
            static const int id = 174;
            typedef Menge::String Type;
        };
        struct ResourceVideo_Name
        {
            static const int id = 176;
            typedef Menge::ConstString Type;
        };
        struct ResourceSound_Name
        {
            static const int id = 178;
            typedef Menge::ConstString Type;
        };
        struct WindowResource_Name
        {
            static const int id = 180;
            typedef Menge::ConstString Type;
        };
        struct ClientSize_Value
        {
            static const int id = 182;
            typedef mt::vec2f Type;
        };
        struct ClientSizeInTiles_Value
        {
            static const int id = 184;
            typedef mt::vec2f Type;
        };
        struct Sequence_Index
        {
            static const int id = 187;
            typedef size_t Type;
        };
        struct Sequence_Delay
        {
            static const int id = 188;
            typedef float Type;
        };
        struct SequenceArray_Count
        {
            static const int id = 190;
            typedef size_t Type;
        };
        struct SequenceArray_Delay
        {
            static const int id = 191;
            typedef float Type;
        };
        struct Folder_Path
        {
            static const int id = 193;
            typedef Menge::String Type;
        };
        struct Fontdef_Path
        {
            static const int id = 195;
            typedef Menge::String Type;
        };
        struct Image_Path
        {
            static const int id = 197;
            typedef Menge::String Type;
        };
        struct OutlineImage_Path
        {
            static const int id = 199;
            typedef Menge::String Type;
        };
        struct Font_height
        {
            static const int id = 200;
            typedef float Type;
        };
        struct Char_width
        {
            static const int id = 202;
            typedef int Type;
        };
        struct Char_code
        {
            static const int id = 203;
            typedef Menge::String Type;
        };
        struct Char_rect
        {
            static const int id = 204;
            typedef Menge::String Type;
        };
        struct Char_offset
        {
            static const int id = 205;
            typedef Menge::String Type;
        };
        struct ImageResource_Name
        {
            static const int id = 207;
            typedef Menge::ConstString Type;
        };
        struct Filter_Value
        {
            static const int id = 209;
            typedef size_t Type;
        };
        struct File_UV
        {
            static const int id = 211;
            typedef mt::vec4f Type;
        };
        struct File_Offset
        {
            static const int id = 212;
            typedef mt::vec2f Type;
        };
        struct File_MaxSize
        {
            static const int id = 213;
            typedef mt::vec2f Type;
        };
        struct File_Size
        {
            static const int id = 214;
            typedef mt::vec2f Type;
        };
        struct File_Alpha
        {
            static const int id = 215;
            typedef bool Type;
        };
        struct File_From
        {
            static const int id = 216;
            typedef int Type;
        };
        struct File_To
        {
            static const int id = 217;
            typedef int Type;
        };
        struct File_Step
        {
            static const int id = 218;
            typedef int Type;
        };
        struct File_WrapX
        {
            static const int id = 219;
            typedef bool Type;
        };
        struct File_WrapY
        {
            static const int id = 220;
            typedef bool Type;
        };
        struct Texture_Name
        {
            static const int id = 222;
            typedef Menge::String Type;
        };
        struct Texture_Color
        {
            static const int id = 223;
            typedef Menge::ColourValue Type;
        };
        struct Mesh_Name
        {
            static const int id = 225;
            typedef Menge::String Type;
        };
        struct Width_Value
        {
            static const int id = 227;
            typedef float Type;
        };
        struct Depth_Value
        {
            static const int id = 229;
            typedef float Type;
        };
        struct ConcaveGeometryFile_Name
        {
            static const int id = 231;
            typedef Menge::String Type;
        };
        struct ConvexGeometryFile_Name
        {
            static const int id = 233;
            typedef Menge::String Type;
        };
        struct Loop_Value
        {
            static const int id = 235;
            typedef bool Type;
        };
        struct Shuffle_Value
        {
            static const int id = 237;
            typedef bool Type;
        };
        struct Track_File
        {
            static const int id = 240;
            typedef Menge::String Type;
        };
        struct Track_Codec
        {
            static const int id = 241;
            typedef Menge::ConstString Type;
        };
        struct IsStreamable_Value
        {
            static const int id = 243;
            typedef bool Type;
        };
        struct ResourceTileSet_Name
        {
            static const int id = 245;
            typedef Menge::String Type;
        };
        struct TileMap_File
        {
            static const int id = 246;
            typedef Menge::String Type;
        };
        struct Tiles_Value
        {
            static const int id = 248;
            typedef size_t Type;
        };
        struct TileSet_File
        {
            static const int id = 250;
            typedef Menge::String Type;
        };
        struct Tile_MinAngle
        {
            static const int id = 252;
            typedef float Type;
        };
        struct Tile_MaxAngle
        {
            static const int id = 253;
            typedef float Type;
        };
        struct Tile_Image
        {
            static const int id = 254;
            typedef Menge::ConstString Type;
        };
        struct Tile_ImageBack
        {
            static const int id = 255;
            typedef Menge::ConstString Type;
        };
        struct Tile_JuncImage
        {
            static const int id = 256;
            typedef Menge::ConstString Type;
        };
        struct TileJunc_Image
        {
            static const int id = 258;
            typedef Menge::ConstString Type;
        };
        struct Background_Image
        {
            static const int id = 260;
            typedef Menge::ConstString Type;
        };
        struct Background_Codec
        {
            static const int id = 261;
            typedef Menge::ConstString Type;
        };
        struct LeftTop_Image
        {
            static const int id = 263;
            typedef Menge::ConstString Type;
        };
        struct LeftTop_Codec
        {
            static const int id = 264;
            typedef Menge::ConstString Type;
        };
        struct Top_Image
        {
            static const int id = 266;
            typedef Menge::ConstString Type;
        };
        struct Top_Codec
        {
            static const int id = 267;
            typedef Menge::ConstString Type;
        };
        struct RightTop_Image
        {
            static const int id = 269;
            typedef Menge::ConstString Type;
        };
        struct RightTop_Codec
        {
            static const int id = 270;
            typedef Menge::ConstString Type;
        };
        struct Right_Image
        {
            static const int id = 272;
            typedef Menge::ConstString Type;
        };
        struct Right_Codec
        {
            static const int id = 273;
            typedef Menge::ConstString Type;
        };
        struct RightBottom_Image
        {
            static const int id = 275;
            typedef Menge::ConstString Type;
        };
        struct RightBottom_Codec
        {
            static const int id = 276;
            typedef Menge::ConstString Type;
        };
        struct Bottom_Image
        {
            static const int id = 278;
            typedef Menge::ConstString Type;
        };
        struct Bottom_Codec
        {
            static const int id = 279;
            typedef Menge::ConstString Type;
        };
        struct LeftBottom_Image
        {
            static const int id = 281;
            typedef Menge::ConstString Type;
        };
        struct LeftBottom_Codec
        {
            static const int id = 282;
            typedef Menge::ConstString Type;
        };
        struct Left_Image
        {
            static const int id = 284;
            typedef Menge::ConstString Type;
        };
        struct Left_Codec
        {
            static const int id = 285;
            typedef Menge::ConstString Type;
        };
        struct Resource_Name
        {
            static const int id = 287;
            typedef Menge::ConstString Type;
        };
        struct Resource_Type
        {
            static const int id = 288;
            typedef Menge::ConstString Type;
        };
        struct Main_Value
        {
            static const int id = 290;
            typedef bool Type;
        };
        struct Size_Value
        {
            static const int id = 292;
            typedef mt::vec2f Type;
        };
        struct Parallax_Factor
        {
            static const int id = 294;
            typedef mt::vec2f Type;
        };
        struct RenderArea_Value
        {
            static const int id = 296;
            typedef Menge::Viewport Type;
        };
        struct RenderViewport_Value
        {
            static const int id = 298;
            typedef Menge::Viewport Type;
        };
        struct GridSize_Value
        {
            static const int id = 300;
            typedef float Type;
        };
        struct RenderTargetName_Value
        {
            static const int id = 302;
            typedef Menge::String Type;
        };
        struct ViewportSize_Value
        {
            static const int id = 304;
            typedef mt::vec2f Type;
        };
        struct Scene_Name
        {
            static const int id = 306;
            typedef Menge::String Type;
        };
        struct Gravity2D_Value
        {
            static const int id = 308;
            typedef mt::vec2f Type;
        };
        struct PhysicWorld2DBox_Value
        {
            static const int id = 310;
            typedef mt::vec4f Type;
        };
        struct PhysicCanSleep_Value
        {
            static const int id = 312;
            typedef bool Type;
        };
        struct RenderTarget_Name
        {
            static const int id = 314;
            typedef Menge::ConstString Type;
        };
        struct RenderTarget_Size
        {
            static const int id = 315;
            typedef mt::vec2f Type;
        };
        struct Text_Key
        {
            static const int id = 316;
            typedef Menge::ConstString Type;
        };
        struct Text_Value
        {
            static const int id = 317;
            typedef Menge::String Type;
        };
        struct Text_CharOffset
        {
            static const int id = 318;
            typedef float Type;
        };
        struct Text_LineOffset
        {
            static const int id = 319;
            typedef float Type;
        };
        struct Text_Font
        {
            static const int id = 320;
            typedef Menge::ConstString Type;
        };
        struct Texts_Font
        {
            static const int id = 322;
            typedef Menge::ConstString Type;
        };
        struct Texts_CharOffset
        {
            static const int id = 323;
            typedef float Type;
        };
        struct Texts_LineOffset
        {
            static const int id = 324;
            typedef float Type;
        };
        struct File_Path
        {
            static const int id = 325;
            typedef Menge::ConstString Type;
        };
        struct File_Codec
        {
            static const int id = 326;
            typedef Menge::ConstString Type;
        };
        struct Frame_UV
        {
            static const int id = 328;
            typedef mt::vec4f Type;
        };
        struct Cell_X
        {
            static const int id = 330;
            typedef std::size_t Type;
        };
        struct Cell_Y
        {
            static const int id = 331;
            typedef std::size_t Type;
        };
        struct Glyph_Resource
        {
            static const int id = 333;
            typedef Menge::ConstString Type;
        };
        struct Resources_Path
        {
            static const int id = 342;
            typedef Menge::String Type;
        };
        struct Arrow_Name
        {
            static const int id = 344;
            typedef Menge::String Type;
        };
        struct Entity_Name
        {
            static const int id = 346;
            typedef Menge::ConstString Type;
        };
        struct Entity_Type
        {
            static const int id = 347;
            typedef Menge::ConstString Type;
        };
        struct Transformation_Position
        {
            static const int id = 349;
            typedef mt::vec2f Type;
        };
        struct Transformation_Direction
        {
            static const int id = 350;
            typedef mt::vec2f Type;
        };
        struct Transformation_Value
        {
            static const int id = 351;
            typedef mt::mat3f Type;
        };
        struct Transformation_Scale
        {
            static const int id = 352;
            typedef mt::vec2f Type;
        };
        struct Transformation_Rotate
        {
            static const int id = 353;
            typedef float Type;
        };
        struct Transformation_Angle
        {
            static const int id = 354;
            typedef float Type;
        };
        struct Transformation_Origin
        {
            static const int id = 355;
            typedef mt::vec2f Type;
        };
        struct Transformation_FixedRotation
        {
            static const int id = 356;
            typedef bool Type;
        };
        struct Scale_Value
        {
            static const int id = 358;
            typedef mt::vec2f Type;
        };
        struct Screensaver_Name
        {
            static const int id = 360;
            typedef Menge::String Type;
        };
        struct Alpha_Value
        {
            static const int id = 362;
            typedef float Type;
        };
    }
}
