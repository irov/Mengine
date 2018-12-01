#include "ResourceValidatePlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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

        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceFile" ), new FactorableUnique<ResourceFileValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ), new FactorableUnique<ResourceHITValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageData" ), new FactorableUnique<ResourceImageDataValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), new FactorableUnique<ResourceImageDefaultValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ), new FactorableUnique<ResourceImageSequenceValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMusic" ), new FactorableUnique<ResourceMusicValidator>() );
        VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), new FactorableUnique<ResourceSoundValidator>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_finalize()
    {
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceFile" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageData" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMusic" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ) );
        
        SERVICE_FINALIZE( Mengine::ResourceValidateServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourceValidateServiceInterface );
    }
}
