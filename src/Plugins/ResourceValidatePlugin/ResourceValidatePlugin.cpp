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
    bool ResourceValidatePlugin::_initializePlugin()
    {
        SERVICE_CREATE( ResourceValidateService );

        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceFile" ), Helper::makeFactorableUnique<ResourceFileValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ), Helper::makeFactorableUnique<ResourceHITValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageData" ), Helper::makeFactorableUnique<ResourceImageDataValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), Helper::makeFactorableUnique<ResourceImageDefaultValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ), Helper::makeFactorableUnique<ResourceImageSequenceValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMusic" ), Helper::makeFactorableUnique<ResourceMusicValidator>() );
        VOCABULARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), Helper::makeFactorableUnique<ResourceSoundValidator>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceFile" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageData" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMusic" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ) );

        SERVICE_FINALIZE( Mengine::ResourceValidateServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroy()
    {
        SERVICE_DESTROY( Mengine::ResourceValidateServiceInterface );
    }
}
