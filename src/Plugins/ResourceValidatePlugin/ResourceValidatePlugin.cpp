#include "ResourceValidatePlugin.h"

#include "ResourceValidateServiceInterface.h"

#include "ResourceImageDefaultValidator.h"
#include "ResourceImageDataValidator.h"
#include "ResourceImageSequenceValidator.h"
#include "ResourceMusicValidator.h"
#include "ResourceSoundValidator.h"
#include "ResourceHITValidator.h"
#include "ResourceFileValidator.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

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
        if( SERVICE_CREATE( ResourceValidateService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceFile" ), Helper::makeFactorableUnique<ResourceFileValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceHIT" ), Helper::makeFactorableUnique<ResourceHITValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageData" ), Helper::makeFactorableUnique<ResourceImageDataValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), Helper::makeFactorableUnique<ResourceImageDefaultValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceImageSequence" ), Helper::makeFactorableUnique<ResourceImageSequenceValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMusic" ), Helper::makeFactorableUnique<ResourceMusicValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), Helper::makeFactorableUnique<ResourceSoundValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

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

        SERVICE_FINALIZE( ResourceValidateService );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ResourceValidateService );
    }
    //////////////////////////////////////////////////////////////////////////
}
