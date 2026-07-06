#include "MetabufPackageContentLoader.h"

#include "Interface/MetabufLoaderServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"
#include "Kernel/Resource.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufPackageContentLoader::MetabufPackageContentLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufPackageContentLoader::~MetabufPackageContentLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPackageContentLoader::enableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( METABUF_LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "resource '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resource '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();
            const ConstString & type = meta_resource->get_Type();

            //ToDo
            bool unique = meta_resource->getd_Unique( true );

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

            MetabufLoaderInterfacePtr loader = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "MetabufLoader" ), type );

            MENGINE_ASSERTION_MEMORY_PANIC( loader, "file '%s' group '%s' resource '%s' type '%s' invalid create loader"
                , Helper::getContentFullPath( _desc.content ).c_str()
                , groupName.c_str()
                , resource->getName().c_str()
                , resource->getType().c_str()
            );

            if( loader->load( resource, meta_resource ) == false )
            {
                LOGGER_ERROR( "file '%s' group '%s' resource '%s' type '%s' invalid load"
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

            bool precompile = meta_resource->getd_Precompile( false );

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

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPackageContentLoader::disableResources( const PackageInterfacePtr & _package, const VectorConstString & _locales, const PackageResourceDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( METABUF_LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "file resources '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse resources '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }

            return false;
        }

        const ConstString & groupName = datablock.get_Name();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Include & includes_include = datablock.get_Includes_Include();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Include & meta_include : includes_include )
        {
            const FilePath & path = meta_include.get_Path();

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

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Resource & includes_resource = datablock.get_Includes_Resource();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Resource * meta_resource : includes_resource )
        {
            const ConstString & name = meta_resource->get_Name();

            ResourcePtr has_resource = nullptr;
            if( RESOURCE_SERVICE()
                ->hasResource( groupName, name, false, &has_resource ) == false )
            {
                LOGGER_ERROR( "path '%s' not found resource name '%s' in group '%s'\nhas resource group '%s' name '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , meta_resource->get_Name().c_str()
                    , groupName.c_str()
                    , has_resource->getGroupName().c_str()
                    , has_resource->getName().c_str()
                );

                return false;
            }

            bool precompile = false;
            meta_resource->get_Precompile( &precompile );

            if( precompile == true )
            {
                has_resource->release();
            }

            RESOURCE_SERVICE()
                ->removeResource( has_resource );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPackageContentLoader::enableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc )
    {
        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( METABUF_LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader : includes_FragmentShader )
        {
            const ConstString & name = meta_FragmentShader.get_Name();

            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const FilePath & fragmentShaderFilePath = meta_FragmentShader.get_File_Path();

            ConstString fileConverterType;
            meta_FragmentShader.get_File_Converter( &fileConverterType );

            bool isCompile = false;
            meta_FragmentShader.get_File_Compile( &isCompile );

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

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader : includes_VertexShader )
        {
            const ConstString & name = meta_VertexShader.get_Name();

            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const FilePath & vertexShaderFilePath = meta_VertexShader.get_File_Path();

            ConstString fileConverterType;
            meta_VertexShader.get_File_Converter( &fileConverterType );

            bool isCompile = false;
            meta_VertexShader.get_File_Compile( &isCompile );

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

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexAttribute & includes_VertexAttribute = datablock.get_Includes_VertexAttribute();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute & meta_VertexAttribute : includes_VertexAttribute )
        {
            const ConstString & renderPlatform = meta_VertexAttribute.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            uint32_t elementSize = meta_VertexAttribute.get_Element_Size();

            const ConstString & name = meta_VertexAttribute.get_Name();

            RenderVertexAttributeInterfacePtr vertexAttribute = RENDERMATERIAL_SERVICE()
                ->createVertexAttribute( name, elementSize, MENGINE_DOCUMENT_FACTORABLE );

            const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::VectorMeta_Attribute & includes_Attributes = meta_VertexAttribute.get_Includes_Attribute();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexAttribute::Meta_Attribute & meta_Attribute : includes_Attributes )
            {
                const ConstString & attribute_uniform = meta_Attribute.get_Uniform();
                uint32_t attribute_index = meta_Attribute.get_Index();
                uint32_t attribute_size = meta_Attribute.get_Size();
                EVertexAttributeType attribute_type = meta_Attribute.get_Type();
                bool attribute_normalized = meta_Attribute.get_Normalized();
                uint32_t attribute_stride = meta_Attribute.get_Stride();
                uint32_t attribute_offset = meta_Attribute.get_Offset();

                vertexAttribute->addAttribute( attribute_uniform, attribute_index, attribute_size, attribute_type, attribute_normalized, attribute_stride, attribute_offset );
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program : includes_Program )
        {
            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            const ConstString & vertexShaderName = meta_Program.get_VertexShader_Name();
            const ConstString & fragmentShaderName = meta_Program.get_FragmentShader_Name();
            const ConstString & vertexAttributeName = meta_Program.get_VertexAttribute_Name();
            uint32_t samplerCount = meta_Program.get_Sampler_Count();

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

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material : includes_Material )
        {
            const ConstString & name = meta_Material.get_Name();

            ConstString renderPlatform;
            if( meta_Material.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            bool is_debug = false;
            meta_Material.get_Debug( &is_debug );

            RenderMaterialStage stage;
            stage.separateAlphaBlendEnable = meta_Material.get_SeparateAlphaBlend_Enable();
            stage.alphaBlendEnable = meta_Material.get_AlphaBlend_Enable();
            stage.depthBufferTestEnable = meta_Material.get_DepthBufferTest_Enable();
            stage.depthBufferWriteEnable = meta_Material.get_DepthBufferWrite_Enable();

            meta_Material.get_BlendFactor_Source( &stage.blendSrc );
            meta_Material.get_BlendFactor_Dest( &stage.blendDst );
            meta_Material.get_BlendFactor_Op( &stage.blendOp );

            meta_Material.get_SeparateAlphaBlendFactor_Source( &stage.separateAlphaBlendSrc );
            meta_Material.get_SeparateAlphaBlendFactor_Dest( &stage.separateAlphaBlendDst );
            meta_Material.get_SeparateAlphaBlendFactor_Op( &stage.separateAlphaBlendOp );

            ConstString programName;
            if( meta_Material.get_Program_Name( &programName ) == true )
            {
                const RenderProgramInterfacePtr & program = RENDERMATERIAL_SERVICE()
                    ->getProgram( programName );

                MENGINE_ASSERTION_MEMORY_PANIC( program, "material '%s' invalid get program '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                    , programName.c_str()
                );

                stage.program = program;
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::VectorMeta_TextureStages & include_TextureStages = meta_Material.get_Includes_TextureStages();

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

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material::Meta_TextureStages & meta_TextureStages : include_TextureStages )
            {
                uint32_t index = meta_TextureStages.get_Stage();

                RenderTextureStage & textureStage = stage.textureStages[index];

                meta_TextureStages.get_AddressMode_U( &textureStage.addressU );
                meta_TextureStages.get_AddressMode_V( &textureStage.addressV );
                meta_TextureStages.get_AddressMode_Border( &textureStage.addressBorder );
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

        RENDERMATERIAL_SERVICE()
            ->updateSolidRenderMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufPackageContentLoader::disableMaterials( const PackageInterfacePtr & _package, const PackageMaterialDesc & _desc )
    {
        MENGINE_UNUSED( _package );

        Metacode::Meta_Data::Meta_DataBlock datablock;

        bool exist = false;
        if( METABUF_LOADER_SERVICE()
            ->load( _desc.content, &datablock, Metacode::Meta_Data::getVersion(), &exist, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "materials '%s' not found"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "invalid parse materials '%s'"
                    , Helper::getContentFullPath( _desc.content ).c_str()
                );
            }

            return false;
        }

        const ConstString & renderPlatformName = RENDER_SYSTEM()
            ->getRenderPlatformName();

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_FragmentShader & includes_FragmentShader = datablock.get_Includes_FragmentShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_FragmentShader & meta_FragmentShader : includes_FragmentShader )
        {
            const ConstString & renderPlatform = meta_FragmentShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_FragmentShader.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeFragmentShader( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_VertexShader & includes_VertexShader = datablock.get_Includes_VertexShader();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_VertexShader & meta_VertexShader : includes_VertexShader )
        {
            const ConstString & renderPlatform = meta_VertexShader.get_RenderPlatform();

            if( renderPlatform != renderPlatformName )
            {
                continue;
            }

            const ConstString & name = meta_VertexShader.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeVertexShader( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Program & includes_Program = datablock.get_Includes_Program();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Program & meta_Program : includes_Program )
        {
            ConstString renderPlatform;
            if( meta_Program.get_RenderPlatform( &renderPlatform ) == true )
            {
                if( renderPlatform != renderPlatformName )
                {
                    continue;
                }
            }

            const ConstString & name = meta_Program.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeProgram( name );
        }

        const Metacode::Meta_Data::Meta_DataBlock::VectorMeta_Material & includes_Material = datablock.get_Includes_Material();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_Material & meta_Material : includes_Material )
        {
            const ConstString & name = meta_Material.get_Name();

            RENDERMATERIAL_SERVICE()
                ->removeMaterialStage( name );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
