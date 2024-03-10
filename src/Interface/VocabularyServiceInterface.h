#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/MixinPointer.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class VocabularyServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "VocabularyService" )

    public:
        virtual void setFactorable( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _value, const DocumentInterfacePtr & _doc ) = 0;
        virtual MixinPointer removeFactorable( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual MixinPointer getFactorable( const ConstString & _category, const ConstString & _prototype ) const = 0;
        virtual bool hasFactorable( const ConstString & _category, const ConstString & _prototype ) const = 0;

    public:
        typedef Lambda<void( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _value )> LambdaVocabulary;
        virtual void foreachVocabulary( const LambdaVocabulary & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_SERVICE()\
    ((Mengine::VocabularyServiceInterface *)SERVICE_GET(Mengine::VocabularyServiceInterface))
//////////////////////////////////////////////////////////////////////////