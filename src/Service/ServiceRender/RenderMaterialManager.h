#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "RenderMaterial.h"

#	include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"
#   include "stdex/stl_map.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_MATERIAL_RENDER_STAGE_MAX
#	define MENGINE_MATERIAL_RENDER_STAGE_MAX 256
#	endif

#	ifndef MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE
#	define MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE 127
#	endif
	//////////////////////////////////////////////////////////////////////////
    class RenderMaterialManager
		: public ServiceBase<RenderMaterialServiceInterface>
    {
    public:
        RenderMaterialManager();
        ~RenderMaterialManager();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool loadMaterials( const ConstString& _pakName, const FilePath& _fileName ) override;
		bool unloadMaterials( const ConstString& _pakName, const FilePath& _fileName ) override;

	public:
		void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) override;

	public:
		const ConstString & getMaterialName( EMaterial _materialId ) const override;

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
		RenderVertexShaderInterfacePtr createVertexShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );
		RenderFragmentShaderInterfacePtr createFragmentShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );

	protected:
		const RenderVertexShaderInterfacePtr & getVertexShader_( const ConstString & _name ) const;
		const RenderFragmentShaderInterfacePtr & getFragmentShader_( const ConstString & _name ) const;

		const RenderProgramInterfacePtr & getProgram_( const ConstString & _name ) const;

    protected:
		uint32_t m_materialEnumerator;

		ETextureFilter m_defaultTextureFilterMipmap;
		ETextureFilter m_defaultTextureFilterMagnification;
		ETextureFilter m_defaultTextureFilterMinification;		

		typedef stdex::map<ConstString, const RenderMaterialStage *> TMapRenderStage;
        TMapRenderStage m_materialStageIndexer;

		RenderMaterialStage m_stages[MENGINE_MATERIAL_RENDER_STAGE_MAX];
		uint32_t m_stageCount;

		typedef stdex::vector<RenderMaterial *> TVectorRenderMaterial;
		TVectorRenderMaterial m_materials[MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE];

		typedef FactoryPoolStore<RenderMaterial, 256> TFactoryRenderMaterial; 
		TFactoryRenderMaterial m_factoryMaterial;

		typedef stdex::vector<uint32_t> TVectorMaterialIndexer;
		TVectorMaterialIndexer m_materialEnumerators;

		RenderMaterialInterfacePtr m_debugMaterial;

		typedef stdex::map<ConstString, RenderVertexShaderInterfacePtr> TMapRenderVertexShaders;
		TMapRenderVertexShaders m_vertexShaders;

		typedef stdex::map<ConstString, RenderFragmentShaderInterfacePtr> TMapRenderFragmentShaders;
		TMapRenderFragmentShaders m_fragmentShaders;

		typedef stdex::map<ConstString, RenderProgramInterfacePtr> TMapRenderPrograms;
		TMapRenderPrograms m_programs;

		ConstString m_defaultStages[EM_MATERIAL_COUNT];
    };
}