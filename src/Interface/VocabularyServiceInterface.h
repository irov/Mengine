#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/MixinPointer.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class VocabularyServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "VocabularyService" )

    public:
        virtual bool setFactorable( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _factorable, const DocumentPtr & _doc ) = 0;
        virtual MixinPointer removeFactorable( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual MixinPointer getFactorable( const ConstString & _category, const ConstString & _prototype ) const = 0;
        virtual bool hasFactorable( const ConstString & _category, const ConstString & _prototype ) const = 0;

    public:
        typedef Lambda<void( const ConstString & _prototype, const MixinPtr & )> LambdaVocabulary;
        virtual void foreachVocabulary( const ConstString & _category, const LambdaVocabulary & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_SERVICE()\
    ((Mengine::VocabularyServiceInterface *)SERVICE_GET(Mengine::VocabularyServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_SET(Interface, Category, Prototype, Factorable, Document)\
    VOCABULARY_SERVICE()->setFactorable(Category, Prototype, Mengine::IntrusivePtr<Interface>::from(Factorable), Document)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_REMOVE(Category, Prototype)\
    VOCABULARY_SERVICE()->removeFactorable(Category, Prototype)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_GET(Category, Prototype)\
    VOCABULARY_SERVICE()->getFactorable(Category, Prototype)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_HAS(Category, Prototype)\
    VOCABULARY_SERVICE()->hasFactorable(Category, Prototype)
//////////////////////////////////////////////////////////////////////////