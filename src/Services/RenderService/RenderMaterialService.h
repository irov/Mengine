#pragma once

#include "Interface/RenderMaterialServiceInterface.h"

#include "RenderMaterial.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/IntrusiveList.h"

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

    protected:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t elementSize, const DocumentPtr & _doc ) override;
        RenderVertexAttributeInterfacePtr removeVertexAttribute( const ConstString & _name ) override;

        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile, const DocumentPtr & _doc ) override;
        RenderVertexShaderInterfacePtr removeVertexShader( const ConstString & _name ) override;

        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _converterType, bool _compile, const DocumentPtr & _doc ) override;
        RenderFragmentShaderInterfacePtr removeFragmentShader( const ConstString & _name ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertexShader, const RenderFragmentShaderInterfacePtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc ) override;
        RenderProgramInterfacePtr removeProgram( const ConstString & _name ) override;

    protected:
        const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const override;
        const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const override;

        const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const override;

    public:
        void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) override;

    public:
        const RenderMaterialStage * createMaterialStage( const ConstString & _name, const RenderMaterialStage & _stage, const DocumentPtr & _doc ) override;
        const RenderMaterialStage * removeMaterialStage( const ConstString & _name ) override;

        const RenderMaterialStage * getMaterialStage( const ConstString & _name ) const override;
        const RenderMaterialStage * cacheMaterialStage( const RenderMaterialStage & _other, const DocumentPtr & _doc ) override;
        void uncacheMaterialStage( const RenderMaterialStage * _stage ) override;

    public:
        void updateSolidRenderMaterial() override;

    public:
        RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentPtr & _doc ) override;

        RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
            , const RenderMaterialStage * _stage
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentPtr & _doc ) override;

        RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentPtr & _doc ) override;

    public:
        const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode, bool _premultiply ) const override;

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
        uint32_t makeMaterialIndex_();
        uint32_t makeMaterialHash_( const ConstString & _materialName, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const;

    protected:
        uint32_t m_materialEnumerator;

        ETextureFilter m_defaultTextureFilterMipmap;
        ETextureFilter m_defaultTextureFilterMagnification;
        ETextureFilter m_defaultTextureFilterMinification;

        typedef Hashtable<ConstString, const RenderMaterialStage *> MapRenderStage;
        MapRenderStage m_materialStageIndexer;

        RenderMaterialStage m_stages[MENGINE_MATERIAL_RENDER_STAGE_MAX];
        uint32_t m_stageCount;

        typedef IntrusiveList<RenderMaterial> IntrusiveListRenderMaterial;
        IntrusiveListRenderMaterial m_materials[MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE];

        RenderMaterialInterfacePtr m_solidRenderMaterial[4][2];

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

        const RenderMaterialStage * m_defaultStages[EM_MATERIAL_COUNT] = {nullptr};
        ConstString m_defaultStageNames[EM_MATERIAL_COUNT];

        typedef Map<ConstString, EMaterial> MapDefaultStagesName;
        MapDefaultStagesName m_defaultStagesEnum;
    };
}