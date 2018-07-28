#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "RenderMaterial.h"

#include "Kernel/Factory.h"

#include "Config/Map.h"

#ifndef MENGINE_MATERIAL_RENDER_STAGE_MAX
#	define MENGINE_MATERIAL_RENDER_STAGE_MAX 256
#endif

#ifndef MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE
#	define MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE 127
#endif

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    class RenderMaterialManager
		: public ServiceBase<RenderMaterialServiceInterface>
    {
    public:
        RenderMaterialManager();
        ~RenderMaterialManager() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadMaterials( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName ) override;
		bool unloadMaterials( const FileGroupInterfacePtr& _fileGroup, const FilePath& _fileName ) override;

    protected:
        const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const override;
        const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const override;
        const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const override;

        const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const override;

	public:
		void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) override;

	public:
		const RenderMaterialStage * cacheStage( const RenderMaterialStage & _other ) override;

	public:
		RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) override;

		RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
			, const RenderMaterialStage * _stage
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) override;

        RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures ) override;

	public:
		void setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial ) override;
		const RenderMaterialInterfacePtr & getDebugMaterial() const override;

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
		RenderVertexShaderInterfacePtr createVertexShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _compile );
		RenderFragmentShaderInterfacePtr createFragmentShader_( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _compile );

    protected:
		uint32_t m_materialEnumerator;

		ETextureFilter m_defaultTextureFilterMipmap;
		ETextureFilter m_defaultTextureFilterMagnification;
		ETextureFilter m_defaultTextureFilterMinification;		

		typedef Map<ConstString, const RenderMaterialStage *> MapRenderStage;
        MapRenderStage m_materialStageIndexer;

		RenderMaterialStage m_stages[MENGINE_MATERIAL_RENDER_STAGE_MAX];
		uint32_t m_stageCount;

		typedef Vector<RenderMaterial *> VectorRenderMaterial;
		VectorRenderMaterial m_materials[MENGINE_RENDER_MATERIAL_HASH_TABLE_SIZE];

		FactoryPtr m_factoryMaterial;

		typedef Vector<uint32_t> VectorMaterialIndexer;
		VectorMaterialIndexer m_materialEnumerators;

		RenderMaterialInterfacePtr m_debugMaterial;

		typedef Map<ConstString, RenderVertexShaderInterfacePtr> MapRenderVertexShaders;
		MapRenderVertexShaders m_vertexShaders;

		typedef Map<ConstString, RenderFragmentShaderInterfacePtr> MapRenderFragmentShaders;
		MapRenderFragmentShaders m_fragmentShaders;

        typedef Map<ConstString, RenderVertexAttributeInterfacePtr> MapRenderVertexAttributes;
        MapRenderVertexAttributes m_vertexAttributes;

		typedef Map<ConstString, RenderProgramInterfacePtr> MapRenderPrograms;
		MapRenderPrograms m_programs;

        const RenderMaterialStage * m_defaultStages[EM_MATERIAL_COUNT];
		ConstString m_defaultStageNames[EM_MATERIAL_COUNT];

        typedef Map<ConstString, EMaterial> MapDefaultStagesName;
        MapDefaultStagesName m_defaultStagesEnum;
    };
}