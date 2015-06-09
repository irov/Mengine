#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "RenderMaterial.h"

#	include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"


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
		
	protected:
		void onRenderMaterialDestroy_( RenderMaterial * _material );

    protected:
        bool createRenderStageGroup( const ConstString & _name, const RenderStage & _stage );

	protected:
		uint32_t makeMaterialIndex_();

	protected:
		GLuint shaderProgram(const char * _vs, const char * _fs, int & transformLocation);

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
    };
}