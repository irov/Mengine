#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "Core/BinaryVector.h"

namespace Menge
{
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
        const RenderMaterialGroup * getMaterialGroup( const ConstString & _name ) const override;

    protected:
        bool createMaterialGroup( const ConstString & _name, const RenderMaterial & _material );
        void removeMaterialGroup( const ConstString & _name );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef BinaryVector<ConstString, RenderMaterialGroup *> TMapMaterialGroup;
        TMapMaterialGroup m_mapMaterialGroup;
    };
}