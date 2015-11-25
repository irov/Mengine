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
#	define MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE 128
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

	public:
		void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) override;

	public:
		const ConstString & getMaterialName( EMaterial _materialId ) const override;

	public:
		const RenderStage * cacheStage( const RenderStage & _other ) override;

	public:
		RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) override;       

		RenderMaterialInterfacePtr getMaterial2( const RenderStage * _stage
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) override;

	public:
		void setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial ) override;
		const RenderMaterialInterfacePtr & getDebugMaterial() const override;
		
	protected:
		void onRenderMaterialDestroy_( RenderMaterial * _material );

    protected:
        bool createRenderStageGroup( const ConstString & _name, const RenderStage & _stage );

	protected:
		uint32_t makeMaterialIndex_();
		uint32_t makeMaterialHash( uint32_t _textureCount, const RenderTextureInterfacePtr * _textures ) const;

	protected:
		bool loadFragmentShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );
		bool loadVertexShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );
			
	protected:
		const RenderShaderInterfacePtr & getVertexShader_( const ConstString & _name ) const;
		const RenderShaderInterfacePtr & getFragmentShader_( const ConstString & _name ) const;

		const RenderProgramInterfacePtr & getProgram_( const ConstString & _name ) const;

    protected:
		uint32_t m_materialEnumerator;

		ETextureFilter m_defaultTextureFilterMipmap;
		ETextureFilter m_defaultTextureFilterMagnification;
		ETextureFilter m_defaultTextureFilterMinification;		

		typedef stdex::map<ConstString, const RenderStage *> TMapRenderStage;
        TMapRenderStage m_stageIndexer;

		RenderStage m_stages[MENGINE_MATERIAL_RENDER_STAGE_MAX];
		uint32_t m_stageCount;

		typedef stdex::vector<RenderMaterial *> TVectorRenderMaterial;
		TVectorRenderMaterial m_materials[MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE];

		typedef FactoryPoolStore<RenderMaterial, 256> TFactoryRenderMaterial; 
		TFactoryRenderMaterial m_factoryMaterial;

		typedef stdex::vector<uint32_t> TVectorMaterialIndexer;
		TVectorMaterialIndexer m_materialIndexer;

		RenderMaterialInterfacePtr m_debugMaterial;

		typedef stdex::map<ConstString, RenderShaderInterfacePtr> TMapRenderShaders;
		TMapRenderShaders m_vertexShaders;
		TMapRenderShaders m_fragmentShaders;

		typedef stdex::map<ConstString, RenderProgramInterfacePtr> TMapRenderPrograms;
		TMapRenderPrograms m_programs;

		ConstString m_defaultStages[EM_MATERIAL_COUNT];
    };
}