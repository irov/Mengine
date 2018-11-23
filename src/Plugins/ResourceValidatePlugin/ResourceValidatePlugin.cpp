#include "ResourceValidatePlugin.h"

#include "Interface/StringizeServiceInterface.h"

#include "ResourceValidateServiceInterface.h"

#include "ResourceImageDefaultValidator.h"
#include "ResourceImageDataValidator.h"
#include "ResourceImageSequenceValidator.h"
#include "ResourceMusicValidator.h"
#include "ResourceSoundValidator.h"
#include "ResourceHITValidator.h"
#include "ResourceFileValidator.h"

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

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceFile" ), new FactorableUnique<ResourceFileValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceHIT" ), new FactorableUnique<ResourceHITValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageData" ), new FactorableUnique<ResourceImageDataValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), new FactorableUnique<ResourceImageDefaultValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ), new FactorableUnique<ResourceImageSequenceValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceMusic" ), new FactorableUnique<ResourceMusicValidator>() );

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceSound" ), new FactorableUnique<ResourceSoundValidator>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_finalize()
    {
        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceFile" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageData" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceMusic" ) );

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidator( STRINGIZE_STRING_LOCAL( "ResourceSound" ) );

        SERVICE_FINALIZE( Mengine::ResourceValidateServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourceValidateServiceInterface );
    }
}
