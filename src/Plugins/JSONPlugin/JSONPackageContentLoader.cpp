#include "JSONPackageContentLoader.h"

#include "Interface/JSONLoaderInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Resource.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONPackageContentLoader::JSONPackageContentLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONPackageContentLoader::~JSONPackageContentLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool loadDataBlockJSON( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc, jpp::object * const _json )
        {
            InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, _doc );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "json DataBlock '%s' not found"
                    , Helper::getContentFullPath( _content ).c_str()
                );

                return false;
            }

            jpp::object json = Helper::loadJSONStream( stream, _doc );

            _content->closeInputStreamFile( stream );

            if( json.invalid() == true )
            {
                LOGGER_ERROR( "invalid parse json DataBlock '%s'"
                    , Helper::getContentFullPath( _content ).c_str()
                );

                return false;
            }

            *_json = json;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getJSONArray( const jpp::object & _json, const Char * _key, jpp::array * const _array )
        {
            jpp::object j_array;
            if( _json.exist( _key, &j_array ) == false )
            {
                return false;
            }

            if( j_array.is_type_array() == false )
            {
                LOGGER_ERROR( "json DataBlock key '%s' is not array"
                    , _key
                );

                return false;
            }

            *_array = jpp::array( j_array );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool checkRenderPlatform( const jpp::object & _json, const ConstString & _renderPlatformName, bool _required )
        {
            ConstString renderPlatform;
            if( Helper::getJSONConstString( _json, "RenderPlatform", &renderPlatform ) == false )
            {
                return _required == false;
            }

            return renderPlatform == _renderPlatformName;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getRequiredConstString( const jpp::object & _json, const Char * _key, ConstString * const _value )
        {
            if( Helper::getJSONConstString( _json, _key, _value ) == false )
            {
                LOGGER_ERROR( "json DataBlock missing required string '%s'"
                    , _key
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getRequiredUInt32( const jpp::object & _json, const Char * _key, uint32_t * const _value )
        {
            if( Helper::getJSONUInt32( _json, _key, _value ) == false )
            {
                LOGGER_ERROR( "json DataBlock missing required uint32 '%s'"
                    , _key
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getRequiredMemberFilePath( const jpp::object & _json, const Char * _node, const Char * _key, FilePath * const _value )
        {
            if( Helper::getJSONMemberFilePath( _json, _node, _key, _value ) == false )
            {
                LOGGER_ERROR( "json DataBlock missing required member '%s.%s'"
                    , _node
                    , _key
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getRequiredMemberConstString( const jpp::object & _json, const Char * _node, const Char * _key, ConstString * const _value )
        {
            if( Helper::getJSONMemberConstString( _json, _node, _key, _value ) == false )
            {
                LOGGER_ERROR( "json DataBlock missing required member '%s.%s'"
                    , _node
                    , _key
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getRequiredMemberUInt32( const jpp::object & _json, const Char * _node, const Char * _key, uint32_t * const _value )
        {
            if( Helper::getJSONMemberUInt32( _json, _node, _key, _value ) == false )
            {
                LOGGER_ERROR( "json DataBlock missing required member '%s.%s'"
                    , _node
                    , _key
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPackageContentLoader::enableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        jpp::object datablock;
        if( Detail::loadDataBlockJSON( _desc.content, MENGINE_DOCUMENT_FACTORABLE, &datablock ) == false )
        {
            return false;
        }

        ConstString groupName;
        if( Detail::getRequiredConstString( datablock, "Name", &groupName ) == false )
        {
            return false;
        }

        jpp::array resources;
        if( Detail::getJSONArray( datablock, "Resource", &resources ) == true )
        {
            for( const jpp::object & meta_resource : resources )
            {
                ConstString name;
                ConstString type;

                if( Detail::getRequiredConstString( meta_resource, "Name", &name ) == false ||
                    Detail::getRequiredConstString( meta_resource, "Type", &type ) == false )
                {
                    return false;
                }

                bool unique = meta_resource.get( "Unique", true );

                ResourcePtr has_resource;
                if( RESOURCE_SERVICE()
                    ->hasResource( groupName, name, true, &has_resource ) == true )
                {
                    if( unique == false )
                    {
                        continue;
                    }

                    LOGGER_ERROR( "file '%s' already exist resource name '%s' in group '%s'\nhas resource group '%s' name '%s'"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , name.c_str()
                        , groupName.c_str()
                        , has_resource->getGroupName().c_str()
                        , has_resource->getName().c_str()
                    );

                    return false;
                }

                ResourceCook cook;
                cook.locales = _locales;
                cook.groupName = groupName;
                cook.name = name;
                cook.type = type;
                cook.groupCache = true;
                cook.keep = true;

                ResourcePtr resource = RESOURCE_SERVICE()
                    ->createResource( cook, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "file '%s' group '%s' resource '%s' type '%s' invalid create"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , groupName.c_str()
                    , name.c_str()
                    , type.c_str()
                );

                resource->setTags( _desc.tags );

                ContentInterfacePtr content = Helper::makeFileContent( _package->getFileGroup(), FilePath::none(), MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( content, "file '%s' group '%s' resource '%s' type '%s' invalid create content"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , groupName.c_str()
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                content->setGroupName( groupName );

                resource->setContent( content );

                JSONLoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "JSONLoader" ), type );

                MENGINE_ASSERTION_MEMORY_PANIC( loader, "file '%s' group '%s' resource '%s' type '%s' invalid create JSON loader"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , groupName.c_str()
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                if( loader->load( resource, meta_resource ) == false )
                {
                    LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid JSON load"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , groupName.c_str()
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return false;
                }

                resource->setIgnored( _desc.ignored );

                if( resource->initialize() == false )
                {
                    LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid initialize"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , groupName.c_str()
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return false;
                }

                bool precompile = meta_resource.get( "Precompile", false );

                if( precompile == true )
                {
                    resource->setPrecompile( true );

                    if( resource->compile() == false )
                    {
                        LOGGER_ERROR( "resource '%s' type [%s] invalid precompile"
                            , name.c_str()
                            , type.c_str()
                        );

                        continue;
                    }
                }
            }
        }

        jpp::array includes;
        if( Detail::getJSONArray( datablock, "Include", &includes ) == true )
        {
            for( const jpp::object & meta_include : includes )
            {
                FilePath path;
                if( Helper::getJSONFilePath( meta_include, "Path", &path ) == false )
                {
                    LOGGER_ERROR( "load '%s' resource invalid include missing Path"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                    );

                    return false;
                }

                ContentInterfacePtr content = Helper::makeFileContent( _package->getFileGroup(), path, MENGINE_DOCUMENT_FACTORABLE );

                PackageResourceDesc desc;
                desc.content = content;
                desc.tags = _desc.tags;
                desc.platform = _desc.platform;
                desc.demand = _desc.demand;
                desc.ignored = _desc.ignored;

                if( this->enableResources( _package, _locales, desc ) == false )
                {
                    LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , path.c_str()
                    );

                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPackageContentLoader::disableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        MENGINE_UNUSED( _locales );

        jpp::object datablock;
        if( Detail::loadDataBlockJSON( _desc.content, MENGINE_DOCUMENT_FACTORABLE, &datablock ) == false )
        {
            return false;
        }

        ConstString groupName;
        if( Detail::getRequiredConstString( datablock, "Name", &groupName ) == false )
        {
            return false;
        }

        jpp::array includes;
        if( Detail::getJSONArray( datablock, "Include", &includes ) == true )
        {
            for( const jpp::object & meta_include : includes )
            {
                FilePath path;
                if( Helper::getJSONFilePath( meta_include, "Path", &path ) == false )
                {
                    LOGGER_ERROR( "load '%s' resource invalid include missing Path"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                    );

                    return false;
                }

                ContentInterfacePtr content = Helper::makeFileContent( _package->getFileGroup(), path, MENGINE_DOCUMENT_FACTORABLE );

                PackageResourceDesc desc;
                desc.content = content;
                desc.tags = _desc.tags;
                desc.platform = _desc.platform;
                desc.demand = _desc.demand;
                desc.ignored = _desc.ignored;

                if( this->disableResources( _package, _locales, desc ) == false )
                {
                    LOGGER_ERROR( "load '%s' resource invalid load include '%s'"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , path.c_str()
                    );

                    return false;
                }
            }
        }

        jpp::array resources;
        if( Detail::getJSONArray( datablock, "Resource", &resources ) == true )
        {
            for( const jpp::object & meta_resource : resources )
            {
                ConstString name;
                if( Detail::getRequiredConstString( meta_resource, "Name", &name ) == false )
                {
                    return false;
                }

                ResourcePtr has_resource = nullptr;
                if( RESOURCE_SERVICE()
                    ->hasResource( groupName, name, false, &has_resource ) == false )
                {
                    LOGGER_ERROR( "path '%s' not found resource name '%s' in group '%s'"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , name.c_str()
                        , groupName.c_str()
                    );

                    return false;
                }

                bool precompile = meta_resource.get( "Precompile", false );

                if( precompile == true )
                {
                    has_resource->release();
                }

                RESOURCE_SERVICE()
                    ->removeResource( has_resource );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPackageContentLoader::enableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc )
    {
        jpp::object datablock;
        if( Detail::loadDataBlockJSON( _desc.content, MENGINE_DOCUMENT_FACTORABLE, &datablock ) == false )
        {
            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        jpp::array fragmentShaders;
        if( Detail::getJSONArray( datablock, "FragmentShader", &fragmentShaders ) == true )
        {
            for( const jpp::object & shaderDesc : fragmentShaders )
            {
                if( Detail::checkRenderPlatform( shaderDesc, renderPlatformName, true ) == false )
                {
                    continue;
                }

                ConstString name;
                FilePath fragmentShaderFilePath;

                if( Detail::getRequiredConstString( shaderDesc, "Name", &name ) == false ||
                    Detail::getRequiredMemberFilePath( shaderDesc, "File", "Path", &fragmentShaderFilePath ) == false )
                {
                    return false;
                }

                ConstString fileConverterType;
                Helper::getJSONMemberConstString( shaderDesc, "File", "Converter", &fileConverterType );

                bool isCompile = false;
                Helper::getJSONMemberBool( shaderDesc, "File", "Compile", &isCompile );

                ContentInterfacePtr fragmentShaderContent = Helper::makeFileContent( _package->getFileGroup(), fragmentShaderFilePath, MENGINE_DOCUMENT_FACTORABLE );

                fragmentShaderContent->setConverterType( fileConverterType );

                RenderFragmentShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                    ->createFragmentShader( name, fragmentShaderContent, isCompile, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' fragment shader '%s' compile [%u]"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , Helper::getContentFullPath( fragmentShaderContent ).c_str()
                    , name.c_str()
                    , isCompile
                );

                MENGINE_UNUSED( shader );
            }
        }

        jpp::array vertexShaders;
        if( Detail::getJSONArray( datablock, "VertexShader", &vertexShaders ) == true )
        {
            for( const jpp::object & shaderDesc : vertexShaders )
            {
                if( Detail::checkRenderPlatform( shaderDesc, renderPlatformName, true ) == false )
                {
                    continue;
                }

                ConstString name;
                FilePath vertexShaderFilePath;

                if( Detail::getRequiredConstString( shaderDesc, "Name", &name ) == false ||
                    Detail::getRequiredMemberFilePath( shaderDesc, "File", "Path", &vertexShaderFilePath ) == false )
                {
                    return false;
                }

                ConstString fileConverterType;
                Helper::getJSONMemberConstString( shaderDesc, "File", "Converter", &fileConverterType );

                bool isCompile = false;
                Helper::getJSONMemberBool( shaderDesc, "File", "Compile", &isCompile );

                ContentInterfacePtr vertexShaderContent = Helper::makeFileContent( _package->getFileGroup(), vertexShaderFilePath, MENGINE_DOCUMENT_FACTORABLE );

                vertexShaderContent->setConverterType( fileConverterType );

                RenderVertexShaderInterfacePtr shader = RENDERMATERIAL_SERVICE()
                    ->createVertexShader( name, vertexShaderContent, isCompile, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( shader, "material '%s' invalid load '%s' vertex shader '%s' compile [%u]"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , Helper::getContentFullPath( vertexShaderContent ).c_str()
                    , name.c_str()
                    , isCompile
                );

                MENGINE_UNUSED( shader );
            }
        }

        jpp::array vertexAttributes;
        if( Detail::getJSONArray( datablock, "VertexAttribute", &vertexAttributes ) == true )
        {
            for( const jpp::object & attributeDesc : vertexAttributes )
            {
                if( Detail::checkRenderPlatform( attributeDesc, renderPlatformName, true ) == false )
                {
                    continue;
                }

                ConstString name;
                uint32_t elementSize;

                if( Detail::getRequiredConstString( attributeDesc, "Name", &name ) == false ||
                    Detail::getRequiredMemberUInt32( attributeDesc, "Element", "Size", &elementSize ) == false )
                {
                    return false;
                }

                RenderVertexAttributeInterfacePtr vertexAttribute = RENDERMATERIAL_SERVICE()
                    ->createVertexAttribute( name, elementSize, MENGINE_DOCUMENT_FACTORABLE );

                jpp::array attributes;
                if( Detail::getJSONArray( attributeDesc, "Attribute", &attributes ) == true )
                {
                    for( const jpp::object & metaAttribute : attributes )
                    {
                        ConstString attribute_uniform;
                        uint32_t attribute_index;
                        uint32_t attribute_size;
                        EVertexAttributeType attribute_type;
                        bool attribute_normalized;
                        uint32_t attribute_stride;
                        uint32_t attribute_offset;

                        if( Detail::getRequiredConstString( metaAttribute, "Uniform", &attribute_uniform ) == false ||
                            Detail::getRequiredUInt32( metaAttribute, "Index", &attribute_index ) == false ||
                            Detail::getRequiredUInt32( metaAttribute, "Size", &attribute_size ) == false ||
                            Helper::getJSONVertexAttributeType( metaAttribute, "Type", &attribute_type ) == false ||
                            Helper::getJSONBool( metaAttribute, "Normalized", &attribute_normalized ) == false ||
                            Detail::getRequiredUInt32( metaAttribute, "Stride", &attribute_stride ) == false ||
                            Detail::getRequiredUInt32( metaAttribute, "Offset", &attribute_offset ) == false )
                        {
                            return false;
                        }

                        vertexAttribute->addAttribute( attribute_uniform, attribute_index, attribute_size, attribute_type, attribute_normalized, attribute_stride, attribute_offset );
                    }
                }
            }
        }

        jpp::array programs;
        if( Detail::getJSONArray( datablock, "Program", &programs ) == true )
        {
            for( const jpp::object & programDesc : programs )
            {
                if( Detail::checkRenderPlatform( programDesc, renderPlatformName, false ) == false )
                {
                    continue;
                }

                ConstString name;
                ConstString vertexShaderName;
                ConstString fragmentShaderName;
                ConstString vertexAttributeName;
                uint32_t samplerCount;

                if( Detail::getRequiredConstString( programDesc, "Name", &name ) == false ||
                    Detail::getRequiredMemberConstString( programDesc, "VertexShader", "Name", &vertexShaderName ) == false ||
                    Detail::getRequiredMemberConstString( programDesc, "FragmentShader", "Name", &fragmentShaderName ) == false ||
                    Detail::getRequiredMemberConstString( programDesc, "VertexAttribute", "Name", &vertexAttributeName ) == false ||
                    Detail::getRequiredMemberUInt32( programDesc, "Sampler", "Count", &samplerCount ) == false )
                {
                    return false;
                }

                const RenderVertexShaderInterfacePtr & vertexShader = RENDERMATERIAL_SERVICE()
                    ->getVertexShader( vertexShaderName );

                MENGINE_ASSERTION_MEMORY_PANIC( vertexShader, "material '%s' program '%s' not found vertex shader '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , name.c_str()
                    , vertexShaderName.c_str()
                );

                const RenderFragmentShaderInterfacePtr & fragmentShader = RENDERMATERIAL_SERVICE()
                    ->getFragmentShader( fragmentShaderName );

                MENGINE_ASSERTION_MEMORY_PANIC( fragmentShader, "material '%s' program '%s' not found fragment shader '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , name.c_str()
                    , fragmentShaderName.c_str()
                );

                const RenderVertexAttributeInterfacePtr & vertexAttribute = RENDERMATERIAL_SERVICE()
                    ->getVertexAttribute( vertexAttributeName );

                MENGINE_ASSERTION_MEMORY_PANIC( vertexAttribute, "material '%s' program '%s' not found vertex attribute '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , name.c_str()
                    , vertexAttributeName.c_str()
                );

                RenderProgramInterfacePtr program = RENDERMATERIAL_SERVICE()
                    ->createProgram( name, vertexShader, fragmentShader, vertexAttribute, samplerCount, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid create program vertex '%s' fragment '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , vertexShaderName.c_str()
                    , fragmentShaderName.c_str()
                );

                MENGINE_UNUSED( program );
            }
        }

        jpp::array materials;
        if( Detail::getJSONArray( datablock, "Material", &materials ) == true )
        {
            for( const jpp::object & materialDesc : materials )
            {
                if( Detail::checkRenderPlatform( materialDesc, renderPlatformName, false ) == false )
                {
                    continue;
                }

                ConstString name;
                if( Detail::getRequiredConstString( materialDesc, "Name", &name ) == false )
                {
                    return false;
                }

                bool is_debug = false;
                Helper::getJSONBool( materialDesc, "Debug", &is_debug );

                RenderMaterialStage stage;
                Helper::getJSONMemberBool( materialDesc, "SeparateAlphaBlend", "Enable", &stage.separateAlphaBlendEnable );
                Helper::getJSONMemberBool( materialDesc, "AlphaBlend", "Enable", &stage.alphaBlendEnable );
                Helper::getJSONMemberBool( materialDesc, "DepthBufferTest", "Enable", &stage.depthBufferTestEnable );
                Helper::getJSONMemberBool( materialDesc, "DepthBufferWrite", "Enable", &stage.depthBufferWriteEnable );

                Helper::getJSONMemberBlendFactor( materialDesc, "BlendFactor", "Source", &stage.blendSrc );
                Helper::getJSONMemberBlendFactor( materialDesc, "BlendFactor", "Dest", &stage.blendDst );
                Helper::getJSONMemberBlendOp( materialDesc, "BlendFactor", "Op", &stage.blendOp );

                Helper::getJSONMemberBlendFactor( materialDesc, "SeparateAlphaBlendFactor", "Source", &stage.separateAlphaBlendSrc );
                Helper::getJSONMemberBlendFactor( materialDesc, "SeparateAlphaBlendFactor", "Dest", &stage.separateAlphaBlendDst );
                Helper::getJSONMemberBlendOp( materialDesc, "SeparateAlphaBlendFactor", "Op", &stage.separateAlphaBlendOp );

                ConstString programName;
                if( Helper::getJSONMemberConstString( materialDesc, "Program", "Name", &programName ) == true )
                {
                    const RenderProgramInterfacePtr & program = RENDERMATERIAL_SERVICE()
                        ->getProgram( programName );

                    MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid get program '%s'"
                        , Helper::getContentFullPath( _desc.content ).c_str()
                        , programName.c_str()
                    );

                    stage.program = program;
                }

                for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
                {
                    RenderTextureStage & textureStage = stage.textureStages[index];

                    textureStage.mipmap = RENDERMATERIAL_SERVICE()
                        ->getDefaultTextureFilterMipmap();

                    textureStage.magnification = RENDERMATERIAL_SERVICE()
                        ->getDefaultTextureFilterMagnification();

                    textureStage.minification = RENDERMATERIAL_SERVICE()
                        ->getDefaultTextureFilterMinification();
                }

                jpp::array textureStages;
                if( Detail::getJSONArray( materialDesc, "TextureStages", &textureStages ) == true )
                {
                    for( const jpp::object & textureStageDesc : textureStages )
                    {
                        uint32_t index;
                        if( Detail::getRequiredUInt32( textureStageDesc, "Stage", &index ) == false )
                        {
                            return false;
                        }

                        if( index >= MENGINE_MAX_TEXTURE_STAGES )
                        {
                            LOGGER_ERROR( "material '%s' texture stage index %u out of range"
                                , name.c_str()
                                , index
                            );

                            return false;
                        }

                        RenderTextureStage & textureStage = stage.textureStages[index];

                        Helper::getJSONMemberTextureAddressMode( textureStageDesc, "AddressMode", "U", &textureStage.addressU );
                        Helper::getJSONMemberTextureAddressMode( textureStageDesc, "AddressMode", "V", &textureStage.addressV );
                        Helper::getJSONMemberUInt32( textureStageDesc, "AddressMode", "Border", &textureStage.addressBorder );
                    }
                }

                const RenderMaterialStage * cache_stage = RENDERMATERIAL_SERVICE()
                    ->createMaterialStage( name, stage, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( cache_stage, "material '%s' invalid create stage group '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , name.c_str()
                );

                MENGINE_UNUSED( cache_stage );

                if( is_debug == true )
                {
                    const RenderMaterialInterfacePtr & debugLineMaterial = RENDERMATERIAL_SERVICE()
                        ->getMaterial( name, PT_LINELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

                    RENDERMATERIAL_SERVICE()
                        ->setDebugLineMaterial( debugLineMaterial );

                    const RenderMaterialInterfacePtr & debugTriangleMaterial = RENDERMATERIAL_SERVICE()
                        ->getMaterial( name, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

                    RENDERMATERIAL_SERVICE()
                        ->setDebugTriangleMaterial( debugTriangleMaterial );
                }
            }
        }

        RENDERMATERIAL_SERVICE()
            ->updateSolidRenderMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONPackageContentLoader::disableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc )
    {
        MENGINE_UNUSED( _package );

        jpp::object datablock;
        if( Detail::loadDataBlockJSON( _desc.content, MENGINE_DOCUMENT_FACTORABLE, &datablock ) == false )
        {
            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        jpp::array fragmentShaders;
        if( Detail::getJSONArray( datablock, "FragmentShader", &fragmentShaders ) == true )
        {
            for( const jpp::object & shaderDesc : fragmentShaders )
            {
                if( Detail::checkRenderPlatform( shaderDesc, renderPlatformName, true ) == false )
                {
                    continue;
                }

                ConstString name;
                if( Detail::getRequiredConstString( shaderDesc, "Name", &name ) == false )
                {
                    return false;
                }

                RENDERMATERIAL_SERVICE()
                    ->removeFragmentShader( name );
            }
        }

        jpp::array vertexShaders;
        if( Detail::getJSONArray( datablock, "VertexShader", &vertexShaders ) == true )
        {
            for( const jpp::object & shaderDesc : vertexShaders )
            {
                if( Detail::checkRenderPlatform( shaderDesc, renderPlatformName, true ) == false )
                {
                    continue;
                }

                ConstString name;
                if( Detail::getRequiredConstString( shaderDesc, "Name", &name ) == false )
                {
                    return false;
                }

                RENDERMATERIAL_SERVICE()
                    ->removeVertexShader( name );
            }
        }

        jpp::array programs;
        if( Detail::getJSONArray( datablock, "Program", &programs ) == true )
        {
            for( const jpp::object & programDesc : programs )
            {
                if( Detail::checkRenderPlatform( programDesc, renderPlatformName, false ) == false )
                {
                    continue;
                }

                ConstString name;
                if( Detail::getRequiredConstString( programDesc, "Name", &name ) == false )
                {
                    return false;
                }

                RENDERMATERIAL_SERVICE()
                    ->removeProgram( name );
            }
        }

        jpp::array materials;
        if( Detail::getJSONArray( datablock, "Material", &materials ) == true )
        {
            for( const jpp::object & materialDesc : materials )
            {
                ConstString name;
                if( Detail::getRequiredConstString( materialDesc, "Name", &name ) == false )
                {
                    return false;
                }

                RENDERMATERIAL_SERVICE()
                    ->removeMaterialStage( name );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
