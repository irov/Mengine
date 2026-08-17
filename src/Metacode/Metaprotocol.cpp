#include "Metaprotocol.h"

namespace Metacode
{
    //////////////////////////////////////////////////////////////////////////
    void MetaprotocolGenerator::generate( Metabuf::ProtocolInterface * _protocol ) const
    {
        _protocol->setVersion( 157U, 3041487559U );

        _protocol->addType( "Color", "Mengine::Color", "float4", false, false, false );
        _protocol->addType( "Color255", "Mengine::Color", "float4inv255", false, false, false );
        _protocol->addType( "Mengine::ConstString", "Mengine::ConstString", "string", false, false, false );
        Metabuf::TypeInterface * type_3 = _protocol->addType( "Mengine::EBlendFactor", "Mengine::EBlendFactor", "uint32_t", true, false, false );
        type_3->addEnumerator( "ONE", "ONE", 0U );
        type_3->addEnumerator( "ZERO", "ZERO", 1U );
        type_3->addEnumerator( "DEST_COLOUR", "DEST_COLOUR", 2U );
        type_3->addEnumerator( "SOURCE_COLOUR", "SOURCE_COLOUR", 3U );
        type_3->addEnumerator( "ONE_MINUS_DEST_COLOUR", "ONE_MINUS_DEST_COLOUR", 4U );
        type_3->addEnumerator( "ONE_MINUS_SOURCE_COLOUR", "ONE_MINUS_SOURCE_COLOUR", 5U );
        type_3->addEnumerator( "DEST_ALPHA", "DEST_ALPHA", 6U );
        type_3->addEnumerator( "SOURCE_ALPHA", "SOURCE_ALPHA", 7U );
        type_3->addEnumerator( "ONE_MINUS_DEST_ALPHA", "ONE_MINUS_DEST_ALPHA", 8U );
        type_3->addEnumerator( "ONE_MINUS_SOURCE_ALPHA", "ONE_MINUS_SOURCE_ALPHA", 9U );
        Metabuf::TypeInterface * type_4 = _protocol->addType( "Mengine::EBlendOp", "Mengine::EBlendOp", "uint32_t", true, false, false );
        type_4->addEnumerator( "ADD", "ADD", 0U );
        type_4->addEnumerator( "SUBTRACT", "SUBTRACT", 1U );
        type_4->addEnumerator( "REVSUBTRACT", "REVSUBTRACT", 2U );
        type_4->addEnumerator( "MIN", "MIN", 3U );
        type_4->addEnumerator( "MAX", "MAX", 4U );
        Metabuf::TypeInterface * type_5 = _protocol->addType( "Mengine::ETextureAddressMode", "Mengine::ETextureAddressMode", "uint32_t", true, false, false );
        type_5->addEnumerator( "WRAP", "WRAP", 0U );
        type_5->addEnumerator( "MIRROR", "MIRROR", 1U );
        type_5->addEnumerator( "CLAMP", "CLAMP", 2U );
        type_5->addEnumerator( "BORDER", "BORDER", 3U );
        type_5->addEnumerator( "MIRRORONCE", "MIRRORONCE", 4U );
        Metabuf::TypeInterface * type_6 = _protocol->addType( "Mengine::ETextureArgument", "Mengine::ETextureArgument", "uint32_t", true, false, false );
        type_6->addEnumerator( "CURRENT", "CURRENT", 0U );
        type_6->addEnumerator( "DIFFUSE", "DIFFUSE", 1U );
        type_6->addEnumerator( "SPECULAR", "SPECULAR", 2U );
        type_6->addEnumerator( "TEXTURE", "TEXTURE", 3U );
        type_6->addEnumerator( "TFACTOR", "TFACTOR", 4U );
        Metabuf::TypeInterface * type_7 = _protocol->addType( "Mengine::ETextureOp", "Mengine::ETextureOp", "uint32_t", true, false, false );
        type_7->addEnumerator( "DISABLE", "DISABLE", 0U );
        type_7->addEnumerator( "SELECTARG1", "SELECTARG1", 1U );
        type_7->addEnumerator( "SELECTARG2", "SELECTARG2", 2U );
        type_7->addEnumerator( "MODULATE", "MODULATE", 3U );
        type_7->addEnumerator( "MODULATE2X", "MODULATE2X", 4U );
        type_7->addEnumerator( "MODULATE4X", "MODULATE4X", 5U );
        type_7->addEnumerator( "ADD", "ADD", 6U );
        type_7->addEnumerator( "SUBTRACT", "SUBTRACT", 7U );
        Metabuf::TypeInterface * type_8 = _protocol->addType( "Mengine::EVertexAttributeType", "Mengine::EVertexAttributeType", "uint32_t", true, false, false );
        type_8->addEnumerator( "FLOAT", "FLOAT", 0U );
        type_8->addEnumerator( "UNSIGNED_BYTE", "UNSIGNED_BYTE", 1U );
        _protocol->addType( "Mengine::FilePath", "Mengine::FilePath", "string", false, false, false );
        _protocol->addType( "Mengine::Floats", "Mengine::Floats", "floats", false, false, false );
        _protocol->addType( "Mengine::Int16s", "Mengine::Int16s", "int16s", false, false, false );
        _protocol->addType( "Mengine::Int32s", "Mengine::Int32s", "int32s", false, false, false );
        _protocol->addType( "Mengine::Int8s", "Mengine::Int8s", "int8s", false, false, false );
        _protocol->addType( "Mengine::Polygon", "Mengine::Polygon", "floats", false, false, false );
        _protocol->addType( "Mengine::String", "Mengine::String", "string", false, false, false );
        _protocol->addType( "Mengine::Tags", "Mengine::Tags", "strings", false, false, false );
        _protocol->addType( "Mengine::UInt16s", "Mengine::UInt16s", "uint16s", false, false, false );
        _protocol->addType( "Mengine::UInt32s", "Mengine::UInt32s", "uint32s", false, false, false );
        _protocol->addType( "Mengine::UInt8s", "Mengine::UInt8s", "uint8s", false, false, false );
        _protocol->addType( "Mengine::Viewport", "Mengine::Viewport", "float4", false, false, false );
        _protocol->addType( "Mengine::WChar", "Mengine::WChar", "wchar_t", false, true, false );
        _protocol->addType( "Mengine::WString", "Mengine::WString", "wstring", false, false, false );
        _protocol->addType( "bool", "bool", "bool", false, true, false );
        _protocol->addType( "float", "float", "float", false, true, false );
        _protocol->addType( "mt::box2f", "mt::box2f", "float4", false, false, false );
        _protocol->addType( "mt::mat4f", "mt::mat4f", "float16", false, false, false );
        _protocol->addType( "mt::uv4f", "mt::uv4f", "float8", false, false, false );
        _protocol->addType( "mt::vec2f", "mt::vec2f", "float2", false, false, false );
        _protocol->addType( "mt::vec3f", "mt::vec3f", "float3", false, false, false );
        _protocol->addType( "mt::vec4f", "mt::vec4f", "float4", false, false, false );
        _protocol->addType( "uint32_t", "uint32_t", "uint32_t", false, true, false );

        Metabuf::MetaInterface * meta_0 = _protocol->addMeta( "Data", 1U );
        Metabuf::MetaInterface * meta_1 = _protocol->addMeta( "TiledMap", 1U );

        Metabuf::NodeInterface * node_0 = _protocol->addNode( 1U, "DataBlock", "", "", 39U, 0U, false, nullptr );
        Metabuf::NodeInterface * node_1 = _protocol->addNode( 4U, "FragmentShader", "", "", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_2 = _protocol->addNode( 1U, "Include", "", "", 0U, 0U, false, node_0 );
        Metabuf::NodeInterface * node_3 = _protocol->addNode( 6U, "Material", "", "", 1U, 12U, false, node_0 );
        Metabuf::NodeInterface * node_4 = _protocol->addNode( 1U, "TextureStages", "", "", 0U, 3U, false, node_3 );
        Metabuf::NodeInterface * node_5 = _protocol->addNode( 5U, "Program", "", "", 0U, 0U, false, node_0 );
        Metabuf::NodeInterface * node_6 = _protocol->addNode( 2U, "VertexAttribute", "", "", 1U, 0U, false, node_0 );
        Metabuf::NodeInterface * node_7 = _protocol->addNode( 1U, "Attribute", "", "", 0U, 0U, false, node_6 );
        Metabuf::NodeInterface * node_8 = _protocol->addNode( 3U, "VertexShader", "", "", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_9 = _protocol->addNode( 7U, "Resource", "Type", "", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_10 = _protocol->addNode( 13U, "ResourceAstralax", "", "Resource", 1U, 4U, false, node_0 );
        Metabuf::NodeInterface * node_11 = _protocol->addNode( 1U, "Atlas", "", "", 0U, 0U, false, node_10 );
        Metabuf::NodeInterface * node_12 = _protocol->addNode( 33U, "ResourceCal3dAnimation", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_13 = _protocol->addNode( 34U, "ResourceCal3dMesh", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_14 = _protocol->addNode( 32U, "ResourceCal3dSkeleton", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_15 = _protocol->addNode( 17U, "ResourceCursorICO", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_16 = _protocol->addNode( 16U, "ResourceCursorSystem", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_17 = _protocol->addNode( 39U, "ResourceExternal", "", "Resource", 0U, 2U, true, node_0 );
        Metabuf::NodeInterface * node_18 = _protocol->addNode( 38U, "ResourceFigma", "", "Resource", 0U, 5U, false, node_0 );
        Metabuf::NodeInterface * node_19 = _protocol->addNode( 18U, "ResourceFile", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_20 = _protocol->addNode( 27U, "ResourceHIT", "", "Resource", 0U, 5U, false, node_0 );
        Metabuf::NodeInterface * node_21 = _protocol->addNode( 8U, "ResourceImageData", "", "Resource", 0U, 9U, false, node_0 );
        Metabuf::NodeInterface * node_22 = _protocol->addNode( 9U, "ResourceImageDefault", "", "Resource", 0U, 10U, false, node_0 );
        Metabuf::NodeInterface * node_23 = _protocol->addNode( 10U, "ResourceImageEmpty", "", "Resource", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_24 = _protocol->addNode( 26U, "ResourceImageSequence", "", "Resource", 1U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_25 = _protocol->addNode( 1U, "Sequence", "", "", 0U, 0U, false, node_24 );
        Metabuf::NodeInterface * node_26 = _protocol->addNode( 29U, "ResourceImageSolid", "", "Resource", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_27 = _protocol->addNode( 12U, "ResourceImageSubstract", "", "Resource", 0U, 7U, false, node_0 );
        Metabuf::NodeInterface * node_28 = _protocol->addNode( 11U, "ResourceImageSubstractRGBAndAlpha", "", "Resource", 0U, 7U, false, node_0 );
        Metabuf::NodeInterface * node_29 = _protocol->addNode( 30U, "ResourceInternalObject", "", "Resource", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_30 = _protocol->addNode( 14U, "ResourceJSON", "", "Resource", 0U, 4U, false, node_0 );
        Metabuf::NodeInterface * node_31 = _protocol->addNode( 28U, "ResourceModel3D", "", "Resource", 0U, 5U, false, node_0 );
        Metabuf::NodeInterface * node_32 = _protocol->addNode( 24U, "ResourceMovie", "", "Resource", 3U, 8U, false, node_0 );
        Metabuf::NodeInterface * node_33 = _protocol->addNode( 3U, "MovieCamera3D", "", "", 0U, 0U, false, node_32 );
        Metabuf::NodeInterface * node_34 = _protocol->addNode( 1U, "MovieLayer2D", "", "", 0U, 16U, false, node_32 );
        Metabuf::NodeInterface * node_35 = _protocol->addNode( 2U, "MovieLayer3D", "", "", 0U, 15U, false, node_32 );
        Metabuf::NodeInterface * node_36 = _protocol->addNode( 23U, "ResourceMovie2", "", "Resource", 1U, 4U, false, node_0 );
        Metabuf::NodeInterface * node_37 = _protocol->addNode( 1U, "Composition", "", "", 2U, 2U, false, node_36 );
        Metabuf::NodeInterface * node_38 = _protocol->addNode( 1U, "Layer", "", "", 0U, 5U, false, node_37 );
        Metabuf::NodeInterface * node_39 = _protocol->addNode( 2U, "SubComposition", "", "", 0U, 0U, false, node_37 );
        Metabuf::NodeInterface * node_40 = _protocol->addNode( 21U, "ResourceMusic", "", "Resource", 0U, 7U, false, node_0 );
        Metabuf::NodeInterface * node_41 = _protocol->addNode( 31U, "ResourceShape", "", "Resource", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_42 = _protocol->addNode( 20U, "ResourceSound", "", "Resource", 0U, 7U, false, node_0 );
        Metabuf::NodeInterface * node_43 = _protocol->addNode( 35U, "ResourceSpineAtlas", "", "Resource", 1U, 4U, false, node_0 );
        Metabuf::NodeInterface * node_44 = _protocol->addNode( 1U, "Image", "", "", 0U, 0U, false, node_43 );
        Metabuf::NodeInterface * node_45 = _protocol->addNode( 36U, "ResourceSpineAtlasTexturepacker", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_46 = _protocol->addNode( 37U, "ResourceSpineSkeleton", "", "Resource", 0U, 5U, false, node_0 );
        Metabuf::NodeInterface * node_47 = _protocol->addNode( 15U, "ResourceTexturepacker", "", "Resource", 0U, 2U, false, node_0 );
        Metabuf::NodeInterface * node_48 = _protocol->addNode( 19U, "ResourceTiledMap", "", "Resource", 0U, 4U, false, node_0 );
        Metabuf::NodeInterface * node_49 = _protocol->addNode( 25U, "ResourceVideo", "", "Resource", 0U, 11U, false, node_0 );
        Metabuf::NodeInterface * node_50 = _protocol->addNode( 22U, "ResourceWindow", "", "Resource", 0U, 3U, false, node_0 );
        Metabuf::NodeInterface * node_51 = _protocol->addNode( 4U, "KeyFramesPack", "", "", 5U, 0U, false, nullptr );
        Metabuf::NodeInterface * node_52 = _protocol->addNode( 3U, "ImageShape", "", "", 1U, 5U, false, node_51 );
        Metabuf::NodeInterface * node_53 = _protocol->addNode( 1U, "Shape", "", "", 0U, 0U, false, node_52 );
        Metabuf::NodeInterface * node_54 = _protocol->addNode( 4U, "KeyFrames2D", "", "", 1U, 2U, false, node_51 );
        Metabuf::NodeInterface * node_55 = _protocol->addNode( 1U, "KeyFrame2D", "", "", 0U, 7U, false, node_54 );
        Metabuf::NodeInterface * node_56 = _protocol->addNode( 5U, "KeyFrames3D", "", "", 1U, 2U, false, node_51 );
        Metabuf::NodeInterface * node_57 = _protocol->addNode( 1U, "KeyFrame3D", "", "", 0U, 8U, false, node_56 );
        Metabuf::NodeInterface * node_58 = _protocol->addNode( 2U, "Polygon", "", "", 0U, 0U, false, node_51 );
        Metabuf::NodeInterface * node_59 = _protocol->addNode( 1U, "TimeRemap", "", "", 0U, 0U, false, node_51 );
        Metabuf::NodeInterface * node_60 = _protocol->addNode( 2U, "Pak", "", "", 8U, 0U, false, nullptr );
        Metabuf::NodeInterface * node_61 = _protocol->addNode( 6U, "Datas", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_62 = _protocol->addNode( 1U, "Data", "", "", 0U, 0U, false, node_61 );
        Metabuf::NodeInterface * node_63 = _protocol->addNode( 3U, "Fonts", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_64 = _protocol->addNode( 1U, "Font", "", "", 0U, 0U, false, node_63 );
        Metabuf::NodeInterface * node_65 = _protocol->addNode( 2U, "Glyphs", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_66 = _protocol->addNode( 1U, "Glyph", "", "", 0U, 0U, false, node_65 );
        Metabuf::NodeInterface * node_67 = _protocol->addNode( 7U, "Materials", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_68 = _protocol->addNode( 1U, "Material", "", "", 0U, 1U, false, node_67 );
        Metabuf::NodeInterface * node_69 = _protocol->addNode( 4U, "Resources", "", "", 1U, 3U, false, node_60 );
        Metabuf::NodeInterface * node_70 = _protocol->addNode( 1U, "Resource", "", "", 0U, 1U, false, node_69 );
        Metabuf::NodeInterface * node_71 = _protocol->addNode( 1U, "Scripts", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_72 = _protocol->addNode( 1U, "Script", "", "", 0U, 3U, false, node_71 );
        Metabuf::NodeInterface * node_73 = _protocol->addNode( 8U, "Settings", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_74 = _protocol->addNode( 1U, "Setting", "", "", 0U, 0U, false, node_73 );
        Metabuf::NodeInterface * node_75 = _protocol->addNode( 5U, "Texts", "", "", 1U, 1U, false, node_60 );
        Metabuf::NodeInterface * node_76 = _protocol->addNode( 1U, "Text", "", "", 0U, 0U, false, node_75 );
        Metabuf::NodeInterface * node_77 = _protocol->addNode( 3U, "Texts", "", "", 1U, 0U, false, nullptr );
        Metabuf::NodeInterface * node_78 = _protocol->addNode( 1U, "Text", "", "", 0U, 4U, false, node_77 );
        Metabuf::NodeInterface * node_79 = _protocol->addNode( 5U, "TiledMap", "", "", 4U, 0U, false, nullptr );
        Metabuf::NodeInterface * node_80 = _protocol->addNode( 4U, "ObjectLayer", "", "", 2U, 0U, false, node_79 );
        Metabuf::NodeInterface * node_81 = _protocol->addNode( 2U, "Object", "", "", 2U, 0U, false, node_80 );
        Metabuf::NodeInterface * node_82 = _protocol->addNode( 2U, "Point", "", "", 0U, 0U, false, node_81 );
        Metabuf::NodeInterface * node_83 = _protocol->addNode( 1U, "Property", "", "", 0U, 0U, false, node_81 );
        Metabuf::NodeInterface * node_84 = _protocol->addNode( 1U, "Property", "", "", 0U, 0U, false, node_80 );
        Metabuf::NodeInterface * node_85 = _protocol->addNode( 1U, "Property", "", "", 0U, 0U, false, node_79 );
        Metabuf::NodeInterface * node_86 = _protocol->addNode( 3U, "TileLayer", "", "", 2U, 0U, false, node_79 );
        Metabuf::NodeInterface * node_87 = _protocol->addNode( 1U, "Property", "", "", 0U, 0U, false, node_86 );
        Metabuf::NodeInterface * node_88 = _protocol->addNode( 2U, "Tile", "", "", 0U, 0U, false, node_86 );
        Metabuf::NodeInterface * node_89 = _protocol->addNode( 2U, "Tileset", "", "", 1U, 0U, false, node_79 );
        Metabuf::NodeInterface * node_90 = _protocol->addNode( 1U, "Image", "", "", 0U, 0U, false, node_89 );

        node_0->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_0->addInclude( node_1 );
        node_0->addInclude( node_2 );
        node_0->addInclude( node_3 );
        node_0->addInclude( node_5 );
        node_0->addInclude( node_6 );
        node_0->addInclude( node_8 );
        node_0->addInheritance( node_9 );
        node_0->addGenerator( node_10 );
        node_0->addGenerator( node_12 );
        node_0->addGenerator( node_13 );
        node_0->addGenerator( node_14 );
        node_0->addGenerator( node_15 );
        node_0->addGenerator( node_16 );
        node_0->addGenerator( node_17 );
        node_0->addGenerator( node_18 );
        node_0->addGenerator( node_19 );
        node_0->addGenerator( node_20 );
        node_0->addGenerator( node_21 );
        node_0->addGenerator( node_22 );
        node_0->addGenerator( node_23 );
        node_0->addGenerator( node_24 );
        node_0->addGenerator( node_26 );
        node_0->addGenerator( node_27 );
        node_0->addGenerator( node_28 );
        node_0->addGenerator( node_29 );
        node_0->addGenerator( node_30 );
        node_0->addGenerator( node_31 );
        node_0->addGenerator( node_32 );
        node_0->addGenerator( node_36 );
        node_0->addGenerator( node_40 );
        node_0->addGenerator( node_41 );
        node_0->addGenerator( node_42 );
        node_0->addGenerator( node_43 );
        node_0->addGenerator( node_45 );
        node_0->addGenerator( node_46 );
        node_0->addGenerator( node_47 );
        node_0->addGenerator( node_48 );
        node_0->addGenerator( node_49 );
        node_0->addGenerator( node_50 );

        node_1->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_1->addMemberAttribute( "File", 1U, "Compile", "bool", false, "" );
        node_1->addMemberAttribute( "File", 0U, "Converter", "Mengine::ConstString", false, "" );
        node_1->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_2->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_3->addAttribute( 0U, "Debug", "bool", false, "" );
        node_3->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_3->addMemberAttribute( "AlphaBlend", 8U, "Enable", "bool", false, "false" );
        node_3->addMemberAttribute( "BlendFactor", 2U, "Dest", "Mengine::EBlendFactor", false, "" );
        node_3->addMemberAttribute( "BlendFactor", 3U, "Op", "Mengine::EBlendOp", false, "" );
        node_3->addMemberAttribute( "BlendFactor", 1U, "Source", "Mengine::EBlendFactor", false, "" );
        node_3->addMemberAttribute( "DepthBufferTest", 9U, "Enable", "bool", false, "false" );
        node_3->addMemberAttribute( "DepthBufferWrite", 10U, "Enable", "bool", false, "false" );
        node_3->addMemberAttribute( "Program", 11U, "Name", "Mengine::ConstString", false, "" );
        node_3->addMemberAttribute( "SeparateAlphaBlend", 7U, "Enable", "bool", false, "false" );
        node_3->addMemberAttribute( "SeparateAlphaBlendFactor", 5U, "Dest", "Mengine::EBlendFactor", false, "" );
        node_3->addMemberAttribute( "SeparateAlphaBlendFactor", 6U, "Op", "Mengine::EBlendOp", false, "" );
        node_3->addMemberAttribute( "SeparateAlphaBlendFactor", 4U, "Source", "Mengine::EBlendFactor", false, "" );
        node_3->addInclude( node_4 );

        node_4->addAttribute( 0U, "Stage", "uint32_t", true, "" );
        node_4->addMemberAttribute( "AddressMode", 2U, "Border", "uint32_t", false, "" );
        node_4->addMemberAttribute( "AddressMode", 0U, "U", "Mengine::ETextureAddressMode", false, "" );
        node_4->addMemberAttribute( "AddressMode", 1U, "V", "Mengine::ETextureAddressMode", false, "" );

        node_5->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_5->addMemberAttribute( "FragmentShader", 0U, "Name", "Mengine::ConstString", true, "" );
        node_5->addMemberAttribute( "Sampler", 0U, "Count", "uint32_t", true, "" );
        node_5->addMemberAttribute( "VertexAttribute", 0U, "Name", "Mengine::ConstString", true, "" );
        node_5->addMemberAttribute( "VertexShader", 0U, "Name", "Mengine::ConstString", true, "" );

        node_6->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_6->addMemberAttribute( "Element", 0U, "Size", "uint32_t", true, "" );
        node_6->addInclude( node_7 );

        node_7->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_7->addAttribute( 0U, "Normalized", "bool", true, "" );
        node_7->addAttribute( 0U, "Offset", "uint32_t", true, "" );
        node_7->addAttribute( 0U, "Size", "uint32_t", true, "" );
        node_7->addAttribute( 0U, "Stride", "uint32_t", true, "" );
        node_7->addAttribute( 0U, "Type", "Mengine::EVertexAttributeType", true, "" );
        node_7->addAttribute( 0U, "Uniform", "Mengine::ConstString", true, "" );

        node_8->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_8->addMemberAttribute( "File", 1U, "Compile", "bool", false, "" );
        node_8->addMemberAttribute( "File", 0U, "Converter", "Mengine::ConstString", false, "" );
        node_8->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_9->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_9->addAttribute( 1U, "Precompile", "bool", false, "" );
        node_9->addAttribute( 0U, "Type", "Mengine::ConstString", true, "" );
        node_9->addAttribute( 0U, "Unique", "bool", false, "" );

        node_10->setInheritance( node_9 );
        node_10->addMemberAttribute( "AtlasCount", 0U, "Value", "uint32_t", true, "" );
        node_10->addMemberAttribute( "File", 2U, "Converter", "Mengine::ConstString", false, "" );
        node_10->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_10->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_10->addInclude( node_11 );

        node_11->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_11->addAttribute( 0U, "ResourceName", "Mengine::ConstString", true, "" );

        node_12->setInheritance( node_9 );
        node_12->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_12->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_13->setInheritance( node_9 );
        node_13->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_13->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_14->setInheritance( node_9 );
        node_14->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_14->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_15->setInheritance( node_9 );
        node_15->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_15->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_16->setInheritance( node_9 );
        node_16->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_16->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_17->setInheritance( node_9 );

        node_18->setInheritance( node_9 );
        node_18->addMemberAttribute( "File", 3U, "Dataflow", "Mengine::ConstString", false, "" );
        node_18->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_18->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_18->addMemberAttribute( "Sidecar", 4U, "Path", "Mengine::FilePath", false, "" );

        node_19->setInheritance( node_9 );
        node_19->addMemberAttribute( "File", 2U, "NoExist", "bool", false, "" );
        node_19->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_20->setInheritance( node_9 );
        node_20->addMemberAttribute( "File", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_20->addMemberAttribute( "File", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_20->addMemberAttribute( "File", 4U, "NoExist", "bool", false, "" );
        node_20->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_21->setInheritance( node_9 );
        node_21->addMemberAttribute( "File", 3U, "Alpha", "bool", false, "" );
        node_21->addMemberAttribute( "File", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_21->addMemberAttribute( "File", 0U, "MaxSize", "mt::vec2f", true, "" );
        node_21->addMemberAttribute( "File", 7U, "NoExist", "bool", false, "" );
        node_21->addMemberAttribute( "File", 6U, "Offset", "mt::vec2f", false, "" );
        node_21->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_21->addMemberAttribute( "File", 4U, "Premultiply", "bool", false, "" );
        node_21->addMemberAttribute( "File", 5U, "Size", "mt::vec2f", false, "" );
        node_21->addMemberAttribute( "File", 8U, "TrimAtlas", "bool", false, "" );

        node_22->setInheritance( node_9 );
        node_22->addMemberAttribute( "File", 4U, "Alpha", "bool", false, "" );
        node_22->addMemberAttribute( "File", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_22->addMemberAttribute( "File", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_22->addMemberAttribute( "File", 0U, "MaxSize", "mt::vec2f", true, "" );
        node_22->addMemberAttribute( "File", 8U, "NoExist", "bool", false, "" );
        node_22->addMemberAttribute( "File", 7U, "Offset", "mt::vec2f", false, "" );
        node_22->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_22->addMemberAttribute( "File", 5U, "Premultiply", "bool", false, "" );
        node_22->addMemberAttribute( "File", 6U, "Size", "mt::vec2f", false, "" );
        node_22->addMemberAttribute( "File", 9U, "TrimAtlas", "bool", false, "" );

        node_23->setInheritance( node_9 );
        node_23->addMemberAttribute( "File", 0U, "MaxSize", "mt::vec2f", true, "" );

        node_24->setInheritance( node_9 );
        node_24->addInclude( node_25 );

        node_25->addAttribute( 0U, "Delay", "float", true, "" );
        node_25->addAttribute( 0U, "ResourceImageName", "Mengine::ConstString", true, "" );

        node_26->setInheritance( node_9 );
        node_26->addMemberAttribute( "Color", 0U, "Value", "Color255", true, "" );
        node_26->addMemberAttribute( "Size", 0U, "Value", "mt::vec2f", true, "" );

        node_27->setInheritance( node_9 );
        node_27->addMemberAttribute( "Image", 3U, "Alpha", "bool", false, "" );
        node_27->addMemberAttribute( "Image", 0U, "MaxSize", "mt::vec2f", true, "" );
        node_27->addMemberAttribute( "Image", 0U, "Name", "Mengine::ConstString", true, "" );
        node_27->addMemberAttribute( "Image", 6U, "Offset", "mt::vec2f", false, "" );
        node_27->addMemberAttribute( "Image", 4U, "Premultiply", "bool", false, "" );
        node_27->addMemberAttribute( "Image", 5U, "Size", "mt::vec2f", false, "" );
        node_27->addMemberAttribute( "Image", 0U, "UV", "mt::uv4f", true, "" );
        node_27->addMemberAttribute( "Image", 2U, "UVRotate", "bool", false, "" );

        node_28->setInheritance( node_9 );
        node_28->addMemberAttribute( "Image", 0U, "MaxSize", "mt::vec2f", true, "" );
        node_28->addMemberAttribute( "Image", 0U, "NameAlpha", "Mengine::ConstString", true, "" );
        node_28->addMemberAttribute( "Image", 0U, "NameRGB", "Mengine::ConstString", true, "" );
        node_28->addMemberAttribute( "Image", 6U, "Offset", "mt::vec2f", false, "" );
        node_28->addMemberAttribute( "Image", 4U, "Premultiply", "bool", false, "" );
        node_28->addMemberAttribute( "Image", 5U, "Size", "mt::vec2f", false, "" );
        node_28->addMemberAttribute( "Image", 0U, "UVAlpha", "mt::uv4f", true, "" );
        node_28->addMemberAttribute( "Image", 3U, "UVAlphaRotate", "bool", false, "" );
        node_28->addMemberAttribute( "Image", 0U, "UVRGB", "mt::uv4f", true, "" );
        node_28->addMemberAttribute( "Image", 2U, "UVRGBRotate", "bool", false, "" );

        node_29->setInheritance( node_9 );
        node_29->addMemberAttribute( "Internal", 0U, "Group", "Mengine::ConstString", true, "" );
        node_29->addMemberAttribute( "Internal", 0U, "Name", "Mengine::ConstString", true, "" );

        node_30->setInheritance( node_9 );
        node_30->addMemberAttribute( "File", 2U, "Converter", "Mengine::ConstString", false, "" );
        node_30->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_30->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_31->setInheritance( node_9 );
        node_31->addMemberAttribute( "File", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_31->addMemberAttribute( "File", 2U, "Dataflow", "Mengine::ConstString", false, "" );
        node_31->addMemberAttribute( "File", 4U, "NoExist", "bool", false, "" );
        node_31->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_31->addMemberAttribute( "Image", 0U, "Resource", "Mengine::ConstString", true, "" );

        node_32->setInheritance( node_9 );
        node_32->addMemberAttribute( "Anchor", 5U, "Point", "mt::vec3f", false, "" );
        node_32->addMemberAttribute( "Bounds", 7U, "Box", "mt::box2f", false, "" );
        node_32->addMemberAttribute( "Duration", 0U, "Value", "float", true, "" );
        node_32->addMemberAttribute( "FrameDuration", 0U, "Value", "float", true, "" );
        node_32->addMemberAttribute( "Height", 0U, "Value", "float", true, "" );
        node_32->addMemberAttribute( "KeyFramesPackPath", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_32->addMemberAttribute( "KeyFramesPackPath", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_32->addMemberAttribute( "KeyFramesPackPath", 0U, "Path", "Mengine::FilePath", true, "" );
        node_32->addMemberAttribute( "Loop", 4U, "Segment", "mt::vec2f", false, "" );
        node_32->addMemberAttribute( "Offset", 6U, "Point", "mt::vec3f", false, "" );
        node_32->addMemberAttribute( "Width", 0U, "Value", "float", true, "" );
        node_32->addInclude( node_33 );
        node_32->addInclude( node_34 );
        node_32->addInclude( node_35 );

        node_33->addAttribute( 0U, "CameraAspect", "float", true, "" );
        node_33->addAttribute( 0U, "CameraFOV", "float", true, "" );
        node_33->addAttribute( 0U, "CameraInterest", "mt::vec3f", true, "" );
        node_33->addAttribute( 0U, "CameraPosition", "mt::vec3f", true, "" );
        node_33->addAttribute( 0U, "Height", "float", true, "" );
        node_33->addAttribute( 0U, "Width", "float", true, "" );

        node_34->addAttribute( 10U, "AnchorPoint", "mt::vec3f", false, "" );
        node_34->addAttribute( 2U, "BlendingMode", "Mengine::ConstString", false, "" );
        node_34->addAttribute( 0U, "In", "float", true, "" );
        node_34->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_34->addAttribute( 9U, "Loop", "bool", false, "" );
        node_34->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_34->addAttribute( 0U, "Out", "float", true, "" );
        node_34->addAttribute( 15U, "Params", "uint32_t", false, "" );
        node_34->addAttribute( 0U, "Parent", "uint32_t", false, "" );
        node_34->addAttribute( 6U, "PlayCount", "uint32_t", false, "" );
        node_34->addAttribute( 5U, "Polygon", "bool", false, "" );
        node_34->addAttribute( 11U, "Position", "mt::vec3f", false, "" );
        node_34->addAttribute( 13U, "Rotation", "mt::vec3f", false, "" );
        node_34->addAttribute( 12U, "Scale", "mt::vec3f", false, "" );
        node_34->addAttribute( 4U, "Shape", "bool", false, "" );
        node_34->addAttribute( 0U, "Source", "Mengine::ConstString", true, "" );
        node_34->addAttribute( 1U, "StartInterval", "float", false, "" );
        node_34->addAttribute( 7U, "Stretch", "float", false, "" );
        node_34->addAttribute( 8U, "Switch", "bool", false, "" );
        node_34->addAttribute( 3U, "TimeRemap", "bool", false, "" );
        node_34->addAttribute( 0U, "Type", "Mengine::ConstString", true, "" );
        node_34->addAttribute( 14U, "Viewport", "Mengine::Viewport", false, "" );

        node_35->addAttribute( 10U, "AnchorPoint", "mt::vec3f", false, "" );
        node_35->addAttribute( 2U, "BlendingMode", "Mengine::ConstString", false, "" );
        node_35->addAttribute( 0U, "In", "float", true, "" );
        node_35->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_35->addAttribute( 9U, "Loop", "bool", false, "" );
        node_35->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_35->addAttribute( 0U, "Out", "float", true, "" );
        node_35->addAttribute( 14U, "Params", "uint32_t", false, "" );
        node_35->addAttribute( 0U, "Parent", "uint32_t", false, "" );
        node_35->addAttribute( 6U, "PlayCount", "uint32_t", false, "" );
        node_35->addAttribute( 5U, "Polygon", "bool", false, "" );
        node_35->addAttribute( 11U, "Position", "mt::vec3f", false, "" );
        node_35->addAttribute( 13U, "Rotation", "mt::vec3f", false, "" );
        node_35->addAttribute( 12U, "Scale", "mt::vec3f", false, "" );
        node_35->addAttribute( 4U, "Shape", "bool", false, "" );
        node_35->addAttribute( 0U, "Source", "Mengine::ConstString", true, "" );
        node_35->addAttribute( 1U, "StartInterval", "float", false, "" );
        node_35->addAttribute( 7U, "Stretch", "float", false, "" );
        node_35->addAttribute( 8U, "Switch", "bool", false, "" );
        node_35->addAttribute( 3U, "TimeRemap", "bool", false, "" );
        node_35->addAttribute( 0U, "Type", "Mengine::ConstString", true, "" );

        node_36->setInheritance( node_9 );
        node_36->addMemberAttribute( "File", 2U, "Dataflow", "Mengine::ConstString", false, "" );
        node_36->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_36->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_36->addInclude( node_37 );

        node_37->addAttribute( 0U, "Bounds", "mt::box2f", false, "" );
        node_37->addAttribute( 0U, "Duration", "float", true, "" );
        node_37->addAttribute( 0U, "FrameDuration", "float", true, "" );
        node_37->addAttribute( 1U, "Master", "bool", false, "" );
        node_37->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_37->addInclude( node_38 );
        node_37->addInclude( node_39 );

        node_38->addAttribute( 1U, "Color", "Color", false, "" );
        node_38->addAttribute( 3U, "Dimension", "mt::box2f", false, "" );
        node_38->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_38->addAttribute( 0U, "Matrix", "mt::mat4f", false, "" );
        node_38->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_38->addAttribute( 2U, "Options", "Mengine::UInt32s", false, "" );
        node_38->addAttribute( 4U, "TrackMatte", "bool", false, "false" );
        node_38->addAttribute( 0U, "Type", "Mengine::ConstString", true, "" );

        node_39->addAttribute( 0U, "Duration", "float", true, "" );
        node_39->addAttribute( 0U, "FrameDuration", "float", true, "" );
        node_39->addAttribute( 0U, "Index", "uint32_t", true, "" );
        node_39->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );

        node_40->setInheritance( node_9 );
        node_40->addMemberAttribute( "DefaultVolume", 6U, "Value", "float", false, "" );
        node_40->addMemberAttribute( "File", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_40->addMemberAttribute( "File", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_40->addMemberAttribute( "File", 4U, "External", "bool", false, "" );
        node_40->addMemberAttribute( "File", 5U, "NoExist", "bool", false, "" );
        node_40->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_41->setInheritance( node_9 );
        node_41->addMemberAttribute( "Polygon", 0U, "Value", "Mengine::Polygon", true, "" );

        node_42->setInheritance( node_9 );
        node_42->addMemberAttribute( "DefaultVolume", 5U, "Value", "float", false, "" );
        node_42->addMemberAttribute( "File", 2U, "Codec", "Mengine::ConstString", false, "" );
        node_42->addMemberAttribute( "File", 3U, "Converter", "Mengine::ConstString", false, "" );
        node_42->addMemberAttribute( "File", 4U, "NoExist", "bool", false, "" );
        node_42->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_42->addMemberAttribute( "IsStreamable", 6U, "Value", "bool", false, "" );

        node_43->setInheritance( node_9 );
        node_43->addMemberAttribute( "File", 2U, "Converter", "Mengine::ConstString", false, "" );
        node_43->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_43->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_43->addInclude( node_44 );

        node_44->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_44->addAttribute( 0U, "Resource", "Mengine::ConstString", true, "" );

        node_45->setInheritance( node_9 );
        node_45->addMemberAttribute( "Texturepacker", 2U, "Name", "Mengine::ConstString", false, "" );

        node_46->setInheritance( node_9 );
        node_46->addMemberAttribute( "Atlas", 4U, "Name", "Mengine::ConstString", false, "" );
        node_46->addMemberAttribute( "File", 2U, "Converter", "Mengine::ConstString", false, "" );
        node_46->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_46->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_47->setInheritance( node_9 );
        node_47->addMemberAttribute( "Image", 0U, "Name", "Mengine::ConstString", true, "" );
        node_47->addMemberAttribute( "JSON", 0U, "Name", "Mengine::ConstString", true, "" );

        node_48->setInheritance( node_9 );
        node_48->addMemberAttribute( "File", 2U, "Converter", "Mengine::ConstString", false, "" );
        node_48->addMemberAttribute( "File", 3U, "NoExist", "bool", false, "" );
        node_48->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );

        node_49->setInheritance( node_9 );
        node_49->addMemberAttribute( "File", 2U, "Alpha", "bool", false, "" );
        node_49->addMemberAttribute( "File", 4U, "Codec", "Mengine::ConstString", false, "" );
        node_49->addMemberAttribute( "File", 5U, "Converter", "Mengine::ConstString", false, "" );
        node_49->addMemberAttribute( "File", 8U, "Duration", "float", false, "" );
        node_49->addMemberAttribute( "File", 7U, "FrameRate", "float", false, "" );
        node_49->addMemberAttribute( "File", 10U, "NoExist", "bool", false, "" );
        node_49->addMemberAttribute( "File", 6U, "NoSeek", "bool", false, "" );
        node_49->addMemberAttribute( "File", 0U, "Path", "Mengine::FilePath", true, "" );
        node_49->addMemberAttribute( "File", 3U, "Premultiply", "bool", false, "" );
        node_49->addMemberAttribute( "File", 9U, "Resize", "float", false, "" );

        node_50->setInheritance( node_9 );
        node_50->addMemberAttribute( "WindowBackground", 2U, "ResourceImageName", "Mengine::ConstString", false, "" );
        node_50->addMemberAttribute( "WindowBottom", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowBottom", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowLeft", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowLeft", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowLeftBottom", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowLeftBottom", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowLeftTop", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowLeftTop", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowRight", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowRight", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowRightBottom", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowRightBottom", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowRightTop", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowRightTop", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );
        node_50->addMemberAttribute( "WindowTop", 0U, "Offset", "mt::vec2f", true, "" );
        node_50->addMemberAttribute( "WindowTop", 0U, "ResourceImageName", "Mengine::ConstString", true, "" );

        node_51->addAttribute( 0U, "MaxIndex", "uint32_t", true, "" );
        node_51->addAttribute( 0U, "Version", "uint32_t", true, "" );
        node_51->addInclude( node_52 );
        node_51->addInclude( node_54 );
        node_51->addInclude( node_56 );
        node_51->addInclude( node_58 );
        node_51->addInclude( node_59 );

        node_52->addAttribute( 4U, "Count", "uint32_t", false, "" );
        node_52->addAttribute( 0U, "ImageMaxSize", "mt::vec2f", true, "" );
        node_52->addAttribute( 1U, "ImageOffset", "mt::vec2f", false, "" );
        node_52->addAttribute( 0U, "ImageSize", "mt::vec2f", false, "" );
        node_52->addAttribute( 2U, "Immutable", "bool", false, "" );
        node_52->addAttribute( 0U, "LayerIndex", "uint32_t", true, "" );
        node_52->addAttribute( 3U, "Subtract", "bool", false, "" );
        node_52->addInclude( node_53 );

        node_53->addAttribute( 0U, "Polygon", "Mengine::Polygon", true, "" );

        node_54->addAttribute( 1U, "Count", "uint32_t", false, "" );
        node_54->addAttribute( 0U, "Immutable", "bool", false, "" );
        node_54->addAttribute( 0U, "LayerIndex", "uint32_t", true, "" );
        node_54->addInclude( node_55 );

        node_55->addAttribute( 0U, "AnchorPoint", "mt::vec2f", false, "" );
        node_55->addAttribute( 6U, "Count", "uint32_t", false, "" );
        node_55->addAttribute( 4U, "Opacity", "float", false, "" );
        node_55->addAttribute( 1U, "Position", "mt::vec2f", false, "" );
        node_55->addAttribute( 3U, "Rotation", "float", false, "" );
        node_55->addAttribute( 2U, "Scale", "mt::vec2f", false, "" );
        node_55->addAttribute( 5U, "Volume", "float", false, "" );

        node_56->addAttribute( 1U, "Count", "uint32_t", false, "" );
        node_56->addAttribute( 0U, "Immutable", "bool", false, "" );
        node_56->addAttribute( 0U, "LayerIndex", "uint32_t", true, "" );
        node_56->addInclude( node_57 );

        node_57->addAttribute( 0U, "AnchorPoint", "mt::vec3f", false, "" );
        node_57->addAttribute( 7U, "Count", "uint32_t", false, "" );
        node_57->addAttribute( 5U, "Opacity", "float", false, "" );
        node_57->addAttribute( 4U, "Orientation", "mt::vec3f", false, "" );
        node_57->addAttribute( 1U, "Position", "mt::vec3f", false, "" );
        node_57->addAttribute( 3U, "Rotation", "mt::vec3f", false, "" );
        node_57->addAttribute( 2U, "Scale", "mt::vec3f", false, "" );
        node_57->addAttribute( 6U, "Volume", "float", false, "" );

        node_58->addAttribute( 0U, "LayerIndex", "uint32_t", true, "" );
        node_58->addAttribute( 0U, "Value", "Mengine::Polygon", true, "" );

        node_59->addAttribute( 0U, "LayerIndex", "uint32_t", true, "" );
        node_59->addAttribute( 0U, "Time", "Mengine::Floats", true, "" );

        node_60->addInclude( node_61 );
        node_60->addInclude( node_63 );
        node_60->addInclude( node_65 );
        node_60->addInclude( node_67 );
        node_60->addInclude( node_69 );
        node_60->addInclude( node_71 );
        node_60->addInclude( node_73 );
        node_60->addInclude( node_75 );

        node_61->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_61->addInclude( node_62 );

        node_62->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_62->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_63->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_63->addInclude( node_64 );

        node_64->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_65->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_65->addInclude( node_66 );

        node_66->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_67->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_67->addInclude( node_68 );

        node_68->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );
        node_68->addAttribute( 0U, "RenderPlatform", "Mengine::ConstString", false, "" );

        node_69->addAttribute( 1U, "Demand", "bool", false, "" );
        node_69->addAttribute( 0U, "Ignored", "bool", false, "" );
        node_69->addAttribute( 2U, "Platform", "Mengine::Tags", false, "" );
        node_69->addInclude( node_70 );

        node_70->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );
        node_70->addAttribute( 0U, "Tags", "Mengine::Tags", false, "" );

        node_71->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_71->addInclude( node_72 );

        node_72->addAttribute( 2U, "Finalizer", "Mengine::ConstString", false, "" );
        node_72->addAttribute( 1U, "Initializer", "Mengine::ConstString", false, "" );
        node_72->addAttribute( 0U, "Module", "Mengine::ConstString", false, "" );
        node_72->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_73->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_73->addInclude( node_74 );

        node_74->addAttribute( 0U, "Name", "Mengine::ConstString", true, "" );
        node_74->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_75->addAttribute( 0U, "Platform", "Mengine::Tags", false, "" );
        node_75->addInclude( node_76 );

        node_76->addAttribute( 0U, "Path", "Mengine::FilePath", true, "" );

        node_77->addInclude( node_78 );

        node_78->addAttribute( 2U, "CharOffset", "float", false, "" );
        node_78->addAttribute( 1U, "Font", "Mengine::ConstString", false, "" );
        node_78->addAttribute( 0U, "Key", "Mengine::ConstString", true, "" );
        node_78->addAttribute( 3U, "LineOffset", "float", false, "" );
        node_78->addAttribute( 0U, "Value", "Mengine::WString", false, "" );

        node_79->addAttribute( 0U, "Height", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "HexSideLength", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "Orientation", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "StaggerAxis", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "StaggerIndex", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "TileHeight", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "TileWidth", "uint32_t", true, "" );
        node_79->addAttribute( 0U, "Width", "uint32_t", true, "" );
        node_79->addInclude( node_80 );
        node_79->addInclude( node_85 );
        node_79->addInclude( node_86 );
        node_79->addInclude( node_89 );

        node_80->addAttribute( 0U, "Id", "uint32_t", true, "" );
        node_80->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_80->addAttribute( 0U, "Offset", "mt::vec2f", true, "" );
        node_80->addAttribute( 0U, "Opacity", "float", true, "" );
        node_80->addAttribute( 0U, "Visible", "bool", true, "" );
        node_80->addInclude( node_81 );
        node_80->addInclude( node_84 );

        node_81->addAttribute( 0U, "Class", "Mengine::String", true, "" );
        node_81->addAttribute( 0U, "Gid", "uint32_t", true, "" );
        node_81->addAttribute( 0U, "Id", "uint32_t", true, "" );
        node_81->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_81->addAttribute( 0U, "Position", "mt::vec2f", true, "" );
        node_81->addAttribute( 0U, "Rotation", "float", true, "" );
        node_81->addAttribute( 0U, "Shape", "uint32_t", true, "" );
        node_81->addAttribute( 0U, "Size", "mt::vec2f", true, "" );
        node_81->addAttribute( 0U, "Type", "Mengine::String", true, "" );
        node_81->addAttribute( 0U, "Visible", "bool", true, "" );
        node_81->addInclude( node_82 );
        node_81->addInclude( node_83 );

        node_82->addAttribute( 0U, "Position", "mt::vec2f", true, "" );

        node_83->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_83->addAttribute( 0U, "Type", "uint32_t", true, "" );
        node_83->addAttribute( 0U, "Value", "Mengine::String", true, "" );

        node_84->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_84->addAttribute( 0U, "Type", "uint32_t", true, "" );
        node_84->addAttribute( 0U, "Value", "Mengine::String", true, "" );

        node_85->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_85->addAttribute( 0U, "Type", "uint32_t", true, "" );
        node_85->addAttribute( 0U, "Value", "Mengine::String", true, "" );

        node_86->addAttribute( 0U, "Id", "uint32_t", true, "" );
        node_86->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_86->addAttribute( 0U, "Offset", "mt::vec2f", true, "" );
        node_86->addAttribute( 0U, "Opacity", "float", true, "" );
        node_86->addAttribute( 0U, "Visible", "bool", true, "" );
        node_86->addInclude( node_87 );
        node_86->addInclude( node_88 );

        node_87->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_87->addAttribute( 0U, "Type", "uint32_t", true, "" );
        node_87->addAttribute( 0U, "Value", "Mengine::String", true, "" );

        node_88->addAttribute( 0U, "Gid", "uint32_t", true, "" );
        node_88->addAttribute( 0U, "Position", "mt::vec2f", true, "" );

        node_89->addAttribute( 0U, "Columns", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "FirstGid", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "ImageHeight", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "ImageWidth", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "Margin", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "Name", "Mengine::String", true, "" );
        node_89->addAttribute( 0U, "Offset", "mt::vec2f", true, "" );
        node_89->addAttribute( 0U, "Resource", "Mengine::String", true, "" );
        node_89->addAttribute( 0U, "Spacing", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "TileCount", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "TileHeight", "uint32_t", true, "" );
        node_89->addAttribute( 0U, "TileWidth", "uint32_t", true, "" );
        node_89->addInclude( node_90 );

        node_90->addAttribute( 0U, "Height", "uint32_t", true, "" );
        node_90->addAttribute( 0U, "LocalId", "uint32_t", true, "" );
        node_90->addAttribute( 0U, "Resource", "Mengine::String", true, "" );
        node_90->addAttribute( 0U, "Width", "uint32_t", true, "" );

        meta_0->addNode( node_0 );
        meta_0->addNode( node_51 );
        meta_0->addNode( node_60 );
        meta_0->addNode( node_77 );
        meta_1->addNode( node_79 );
        _protocol->addInternal( "ResourceImageData" );
        _protocol->addInternal( "ResourceImageDefault" );
        _protocol->addInternal( "ResourceImageEmpty" );
        _protocol->addInternal( "ResourceImageSubstractRGBAndAlpha" );
        _protocol->addInternal( "ResourceImageSubstract" );
        _protocol->addInternal( "ResourceAstralax" );
        _protocol->addInternal( "ResourceJSON" );
        _protocol->addInternal( "ResourceTexturepacker" );
        _protocol->addInternal( "ResourceCursorSystem" );
        _protocol->addInternal( "ResourceCursorICO" );
        _protocol->addInternal( "ResourceFile" );
        _protocol->addInternal( "ResourceTiledMap" );
        _protocol->addInternal( "ResourceSound" );
        _protocol->addInternal( "ResourceMusic" );
        _protocol->addInternal( "ResourceWindow" );
        _protocol->addInternal( "ResourceMovie2" );
        _protocol->addInternal( "ResourceMovie" );
        _protocol->addInternal( "ResourceVideo" );
        _protocol->addInternal( "ResourceImageSequence" );
        _protocol->addInternal( "ResourceHIT" );
        _protocol->addInternal( "ResourceModel3D" );
        _protocol->addInternal( "ResourceImageSolid" );
        _protocol->addInternal( "ResourceInternalObject" );
        _protocol->addInternal( "ResourceShape" );
        _protocol->addInternal( "ResourceCal3dSkeleton" );
        _protocol->addInternal( "ResourceCal3dAnimation" );
        _protocol->addInternal( "ResourceCal3dMesh" );
        _protocol->addInternal( "ResourceSpineAtlas" );
        _protocol->addInternal( "ResourceSpineAtlasTexturepacker" );
        _protocol->addInternal( "ResourceSpineSkeleton" );
        _protocol->addInternal( "ResourceFigma" );
        _protocol->addInternal( "ResourceExternal" );
    }
    //////////////////////////////////////////////////////////////////////////
}
