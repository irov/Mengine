#pragma once

#include "Interface/RenderSystemInterface.h"

#include "RenderMaterial.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Hashtable.h"

#include "Config/Vector.h"
#include "Config/Map.h"

#include "stdex/intrusive_list.h"

#ifndef MENGINE_MATERIAL_RENDER_STAGE_MAX
#define MENGINE_MATERIAL_RENDER_STAGE_MAX 256
#endif

#ifndef MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE
#define MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE 1021
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialService
        : public ServiceBase<RenderMaterialServiceInterface>
    {
    public:
        RenderMaterialService();
        ~RenderMaterialService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadMaterials( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName ) override;
        bool unloadMaterials( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName ) override;

    protected:
        const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const override;
        const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const override;

        const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const override;

    public:
        void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) override;

    public:
        const RenderMaterialStage * getMaterialStage( const ConstString & _materialName ) const override;

        const RenderMaterialStage * cacheStage( const RenderMaterialStage & _other ) override;

    public:
        RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) override;

        RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
            , const RenderMaterialStage * _stage
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) override;

        RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) override;

    public:
        const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode ) const override;

    public:
        void setDebugLineMaterial( const RenderMaterialInterfacePtr & _debugLineMaterial ) override;
        const RenderMaterialInterfacePtr & getDebugLineMaterial() const override;

        void setDebugTriangleMaterial( const RenderMaterialInterfacePtr & _debugTriangleMaterial ) override;
        const RenderMaterialInterfacePtr & getDebugTriangleMaterial() const override;

    public:
        ETextureFilter getDefaultTextureFilterMipmap() const override;
        ETextureFilter getDefaultTextureFilterMagnification() const override;
        ETextureFilter getDefaultTextureFilterMinification() const override;

    protected:
        void onRenderMaterialDestroy_( RenderMaterial * _material );

    protected:
        const RenderMaterialStage * createRenderStageGroup( const ConstString & _name, const RenderMaterialStage & _stage );

    protected:
        uint32_t makeMaterialIndex_();
        uint32_t makeMaterialHash( const ConstString & _materialName, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const;

    protected:
        RenderVertexAttributeInterfacePtr createVertexAttribute_( const ConstString & _name, uint32_t elementSize );
        RenderVertexShaderInterfacePtr createVertexShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile );
        RenderFragmentShaderInterfacePtr createFragmentShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile );

    protected:
        uint32_t m_materialEnumerator;

        ETextureFilter m_defaultTextureFilterMipmap;
        ETextureFilter m_defaultTextureFilterMagnification;
        ETextureFilter m_defaultTextureFilterMinification;

        typedef Hashtable<ConstString, const RenderMaterialStage *> MapRenderStage;
        MapRenderStage m_materialStageIndexer;

        RenderMaterialStage m_stages[MENGINE_MATERIAL_RENDER_STAGE_MAX];
        uint32_t m_stageCount;

        typedef stdex::intrusive_list<RenderMaterial> IntrusiveListRenderMaterial;
        IntrusiveListRenderMaterial m_materials[MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE];

        RenderMaterialInterfacePtr m_solidRenderMaterial[4];

        FactoryPtr m_factoryMaterial;

        typedef Vector<uint32_t> VectorMaterialIndexer;
        VectorMaterialIndexer m_materialEnumerators;

        RenderMaterialInterfacePtr m_debugLineMaterial;
        RenderMaterialInterfacePtr m_debugTriangleMaterial;

        typedef Hashtable<ConstString, RenderVertexShaderInterfacePtr> HashtableRenderVertexShaders;
        HashtableRenderVertexShaders m_vertexShaders;

        typedef Hashtable<ConstString, RenderFragmentShaderInterfacePtr> HashtableRenderFragmentShaders;
        HashtableRenderFragmentShaders m_fragmentShaders;

        typedef Hashtable<ConstString, RenderVertexAttributeInterfacePtr> HashtableRenderVertexAttributes;
        HashtableRenderVertexAttributes m_vertexAttributes;

        typedef Hashtable<ConstString, RenderProgramInterfacePtr> HashtableRenderPrograms;
        HashtableRenderPrograms m_programs;

        const RenderMaterialStage * m_defaultStages[EM_MATERIAL_COUNT];
        ConstString m_defaultStageNames[EM_MATERIAL_COUNT];

        typedef Map<ConstString, EMaterial> MapDefaultStagesName;
        MapDefaultStagesName m_defaultStagesEnum;
    };
}