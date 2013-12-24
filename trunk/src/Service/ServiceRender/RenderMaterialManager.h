#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "Factory/FactoryPool.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct RenderStageGroup
		: public Factorable
	{
		RenderStage stage[4];
	};
	//////////////////////////////////////////////////////////////////////////
	struct RenderMaterialGroup
		: public Factorable
	{
		size_t refcount;

		RenderMaterial material;
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
        bool loadMaterials( const ConstString& _pakName, const FilePath& _filename ) override;

	public:
		const RenderMaterial * getMaterial( const ConstString & _name
			, bool _wrapU
			, bool _wrapV
			, EPrimitiveType _primitiveType
			, size_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) override;        

		void releaseMaterial( const RenderMaterial * _material ) override;

    protected:
        bool createRenderStageGroup( const ConstString & _name, const RenderStage & _stage );

    protected:
        ServiceProviderInterface * m_serviceProvider;

		size_t m_materialEnumerator;

        typedef stdex::binary_vector<ConstString, RenderStageGroup *> TMapRenderStage;
        TMapRenderStage m_stages;

		typedef FactoryPool<RenderStageGroup, 256> TFactoryRenderStage; 
		TFactoryRenderStage m_factoryStage;

		typedef std::vector<RenderMaterialGroup *> TVectorRenderMaterial;
		TVectorRenderMaterial m_materials;

		typedef FactoryPool<RenderMaterialGroup, 256> TFactoryRenderMaterial; 
		TFactoryRenderMaterial m_factoryMaterial;
    };
}