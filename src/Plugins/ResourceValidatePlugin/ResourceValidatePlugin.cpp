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
PLUGIN_FACTORY( ResourceValidate, Mengine::ResourceValidatePlugin );
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

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceFile::getFactorableType(), Helper::makeFactorableUnique<ResourceFileValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceHIT::getFactorableType(), Helper::makeFactorableUnique<ResourceHITValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageData::getFactorableType(), Helper::makeFactorableUnique<ResourceImageDataValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageDefault::getFactorableType(), Helper::makeFactorableUnique<ResourceImageDefaultValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageSequence::getFactorableType(), Helper::makeFactorableUnique<ResourceImageSequenceValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceMusic::getFactorableType(), Helper::makeFactorableUnique<ResourceMusicValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceSound::getFactorableType(), Helper::makeFactorableUnique<ResourceSoundValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceFile::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceHIT::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageData::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageDefault::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceImageSequence::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceMusic::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceSound::getFactorableType() );

        SERVICE_FINALIZE( ResourceValidateService );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceValidatePlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ResourceValidateService );
    }
    //////////////////////////////////////////////////////////////////////////
}
