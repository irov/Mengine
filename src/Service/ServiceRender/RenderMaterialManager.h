#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "RenderMaterial.h"

#	include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"
#   include "stdex/stl_map.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE
#	define MENGE_RENDER_MATERIAL_HASH_TABLE_SIZE 128
#	endif
	//////////////////////////////////////////////////////////////////////////
	struct RenderStageGroup
		: public Factorable
	{
		RenderStage stage[4];
	};
	//////////////////////////////////////////////////////////////////////////
    class RenderMaterialManager
        : public RenderMaterialServiceInterface
    {
    public:
        RenderMaterialManager();
        ~RenderMaterialManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _provider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool loadMaterials( const ConstString& _pakName, const FilePath& _fileName ) override;

	public:
		RenderMaterialInterfacePtr getMaterial( const ConstString & _stageName
			, bool _wrapU
			, bool _wrapV
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

	protected:
		bool loadFragmentShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );
		bool loadVertexShader_( const ConstString & _name, const ConstString & _pakName, const ConstString & _filePath, bool isCompile );
			
	protected:
		const RenderShaderInterfacePtr & getVertexShader_( const ConstString & _name ) const;
		const RenderShaderInterfacePtr & getFragmentShader_( const ConstString & _name ) const;

		const RenderProgramInterfacePtr & getProgram_( const ConstString & _name ) const;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		uint32_t m_materialEnumerator;

        typedef stdex::binary_vector<ConstString, RenderStageGroup *> TMapRenderStage;
        TMapRenderStage m_stages;

		typedef FactoryPoolStore<RenderStageGroup, 256> TFactoryRenderStage; 
		TFactoryRenderStage m_factoryStage;

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
    };
}