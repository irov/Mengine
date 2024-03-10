#include "EasingService.h"

#include "EasingHelper.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EasingService, Mengine::EasingService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EasingService::EasingService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EasingService::~EasingService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EasingService::_initializeService()
    {
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), ConstString::none(), Helper::makeFactorableUnique<EasingLinear>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyLinear" ), Helper::makeFactorableUnique<EasingLinear>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyIn" ), Helper::makeFactorableUnique<EasingIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyOut" ), Helper::makeFactorableUnique<EasingOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyInOut" ), Helper::makeFactorableUnique<EasingInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineIn" ), Helper::makeFactorableUnique<EasingSineIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineOut" ), Helper::makeFactorableUnique<EasingSineOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineInOut" ), Helper::makeFactorableUnique<EasingSineInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadIn" ), Helper::makeFactorableUnique<EasingQuadIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadOut" ), Helper::makeFactorableUnique<EasingQuadOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadInOut" ), Helper::makeFactorableUnique<EasingQuadInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicIn" ), Helper::makeFactorableUnique<EasingCubicIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicOut" ), Helper::makeFactorableUnique<EasingCubicOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicInOut" ), Helper::makeFactorableUnique<EasingCubicInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartIn" ), Helper::makeFactorableUnique<EasingQuartIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartOut" ), Helper::makeFactorableUnique<EasingQuartOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartInOut" ), Helper::makeFactorableUnique<EasingQuartInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintIn" ), Helper::makeFactorableUnique<EasingQuintIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintOut" ), Helper::makeFactorableUnique<EasingQuintOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintInOut" ), Helper::makeFactorableUnique<EasingQuintInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoIn" ), Helper::makeFactorableUnique<EasingExpoIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoOut" ), Helper::makeFactorableUnique<EasingExpoOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoInOut" ), Helper::makeFactorableUnique<EasingExpoInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircIn" ), Helper::makeFactorableUnique<EasingCircIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircOut" ), Helper::makeFactorableUnique<EasingCircOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircInOut" ), Helper::makeFactorableUnique<EasingCircInOut>(MENGINE_DOCUMENT_FACTORABLE), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticIn" ), Helper::makeFactorableUnique<EasingElasticIn>(MENGINE_DOCUMENT_FACTORABLE), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticOut" ), Helper::makeFactorableUnique<EasingElasticOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticInOut" ), Helper::makeFactorableUnique<EasingElasticInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackIn" ), Helper::makeFactorableUnique<EasingBackIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackOut" ), Helper::makeFactorableUnique<EasingBackOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackInOut" ), Helper::makeFactorableUnique<EasingBackInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceIn" ), Helper::makeFactorableUnique<EasingBounceIn>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceOut" ), Helper::makeFactorableUnique<EasingBounceOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceInOut" ), Helper::makeFactorableUnique<EasingBounceInOut>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EasingService::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), ConstString::none() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyLinear" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceInOut" ) );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "Easing" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}