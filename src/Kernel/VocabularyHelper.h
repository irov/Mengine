#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/MixinPointer.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    namespace Helper
    {
        void vocabularySet( const ConstString & _category, const ConstString & _key, const MixinPtr & _value, const DocumentInterfacePtr & _doc );
        MixinPointer vocabularyRemove( const ConstString & _category, const ConstString & _prototype );
        bool vocabularyHas( const ConstString & _category, const ConstString & _prototype );
        MixinPointer vocabularyGet( const ConstString & _category, const ConstString & _prototype );
    }
}
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_SET(T, Category, Prototype, Value, Document)\
    Mengine::Helper::vocabularySet(Category, Prototype, Mengine::IntrusivePtr<T>::from(Value), Document)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_REMOVE(Category, Prototype)\
    Mengine::Helper::vocabularyRemove(Category, Prototype)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_GET(Category, Prototype)\
    Mengine::Helper::vocabularyGet(Category, Prototype)
//////////////////////////////////////////////////////////////////////////
#define VOCABULARY_HAS(Category, Prototype)\
    Mengine::Helper::vocabularyHas(Category, Prototype)
//////////////////////////////////////////////////////////////////////////