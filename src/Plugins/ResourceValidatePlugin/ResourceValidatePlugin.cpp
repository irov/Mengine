#include "ResourceValidatePlugin.h"

#include "ResourceValidateInterface.h"

#include "ResourceValidateVisitor.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourceValidateService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourceValidate, Mengine::ResourceValidatePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceValidatePlugin::ResourceValidatePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceValidatePlugin::~ResourceValidatePlugin()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidatePlugin::_avaliable()
    {
        return true;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceValidatePlugin::_initialize()
    {
        SERVICE_CREATE( ResourceValidateService );

        VisitorPtr resourceValidateVisitor = new FactorableUnique<ResourceValidateVisitor>();

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( resourceValidateVisitor );

        m_resourceValidateVisitor = resourceValidateVisitor;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_finalize()
    {
        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidateVisitor( m_resourceValidateVisitor );

        m_resourceValidateVisitor = nullptr;

        SERVICE_FINALIZE( Mengine::ResourceValidateServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourceValidateServiceInterface );
    }
}
