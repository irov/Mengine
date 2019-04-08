#include "EasingService.h"

#include "EasingHelper.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"

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
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), ConstString::none(), new FactorableUnique<EasingLinear>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyLinear" ), new FactorableUnique<EasingLinear>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyIn" ), new FactorableUnique<EasingIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeOut" ), new FactorableUnique<EasingOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyInOut" ), new FactorableUnique<EasingInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineIn" ), new FactorableUnique<EasingSineIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easeSineOut" ), new FactorableUnique<EasingSineOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easySineInOut" ), new FactorableUnique<EasingSineInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadIn" ), new FactorableUnique<EasingQuadIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadOut" ), new FactorableUnique<EasingQuadOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuadInOut" ), new FactorableUnique<EasingQuadInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicIn" ), new FactorableUnique<EasingCubicIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicOut" ), new FactorableUnique<EasingCubicOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCubicInOut" ), new FactorableUnique<EasingCubicInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartIn" ), new FactorableUnique<EasingQuartIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartOut" ), new FactorableUnique<EasingQuartOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuartInOut" ), new FactorableUnique<EasingQuartInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintIn" ), new FactorableUnique<EasingQuintIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintOut" ), new FactorableUnique<EasingQuintOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyQuintInOut" ), new FactorableUnique<EasingQuintInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoIn" ), new FactorableUnique<EasingExpoIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoOut" ), new FactorableUnique<EasingExpoOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyExpoInOut" ), new FactorableUnique<EasingExpoInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircIn" ), new FactorableUnique<EasingCircIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircOut" ), new FactorableUnique<EasingCircOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyCircInOut" ), new FactorableUnique<EasingCircInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticIn" ), new FactorableUnique<EasingElasticIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticOut" ), new FactorableUnique<EasingElasticOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyElasticInOut" ), new FactorableUnique<EasingElasticInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackIn" ), new FactorableUnique<EasingBackIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackOut" ), new FactorableUnique<EasingBackOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBackInOut" ), new FactorableUnique<EasingBackInOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceIn" ), new FactorableUnique<EasingBounceIn>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceOut" ), new FactorableUnique<EasingBounceOut>() );
        VOCABULARY_SET( EasingInterface, STRINGIZE_STRING_LOCAL( "Easing" ), STRINGIZE_STRING_LOCAL( "easyBounceInOut" ), new FactorableUnique<EasingBounceInOut>() );

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