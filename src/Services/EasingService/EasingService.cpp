#include "EasingService.h"

#include "EasingHelper.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), ConstString::none(), Helper::makeFactorableUnique<EasingLinear>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyLinear" ), Helper::makeFactorableUnique<EasingLinear>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyIn" ), Helper::makeFactorableUnique<EasingIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeOut" ), Helper::makeFactorableUnique<EasingOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyInOut" ), Helper::makeFactorableUnique<EasingInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineIn" ), Helper::makeFactorableUnique<EasingSineIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeSineOut" ), Helper::makeFactorableUnique<EasingSineOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineInOut" ), Helper::makeFactorableUnique<EasingSineInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadIn" ), Helper::makeFactorableUnique<EasingQuadIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadOut" ), Helper::makeFactorableUnique<EasingQuadOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadInOut" ), Helper::makeFactorableUnique<EasingQuadInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicIn" ), Helper::makeFactorableUnique<EasingCubicIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicOut" ), Helper::makeFactorableUnique<EasingCubicOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicInOut" ), Helper::makeFactorableUnique<EasingCubicInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartIn" ), Helper::makeFactorableUnique<EasingQuartIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartOut" ), Helper::makeFactorableUnique<EasingQuartOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartInOut" ), Helper::makeFactorableUnique<EasingQuartInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintIn" ), Helper::makeFactorableUnique<EasingQuintIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintOut" ), Helper::makeFactorableUnique<EasingQuintOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintInOut" ), Helper::makeFactorableUnique<EasingQuintInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoIn" ), Helper::makeFactorableUnique<EasingExpoIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoOut" ), Helper::makeFactorableUnique<EasingExpoOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoInOut" ), Helper::makeFactorableUnique<EasingExpoInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircIn" ), Helper::makeFactorableUnique<EasingCircIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircOut" ), Helper::makeFactorableUnique<EasingCircOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircInOut" ), Helper::makeFactorableUnique<EasingCircInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticIn" ), Helper::makeFactorableUnique<EasingElasticIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticOut" ), Helper::makeFactorableUnique<EasingElasticOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticInOut" ), Helper::makeFactorableUnique<EasingElasticInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackIn" ), Helper::makeFactorableUnique<EasingBackIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackOut" ), Helper::makeFactorableUnique<EasingBackOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackInOut" ), Helper::makeFactorableUnique<EasingBackInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceIn" ), Helper::makeFactorableUnique<EasingBounceIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceOut" ), Helper::makeFactorableUnique<EasingBounceOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceInOut" ), Helper::makeFactorableUnique<EasingBounceInOut>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EasingService::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), ConstString::none() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyLinear" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyInOut" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineIn" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeSineOut" ) );
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
}