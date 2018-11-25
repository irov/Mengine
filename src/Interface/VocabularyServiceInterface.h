#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/MixinPointer.h"
#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class VocabularyServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "VocalubaryService" )

    public:
        virtual bool setFactorable( const ConstString & _category, const ConstString & _prototype, const MixinPtr & _factorable ) = 0;
        virtual bool removeFactorable( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual MixinPointer getFactorable( const ConstString & _category, const ConstString & _prototype ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define VOCALUBARY_SERVICE()\
	((Mengine::VocabularyServiceInterface *)SERVICE_GET(Mengine::VocabularyServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define VOCALUBARY_SET(Interface, Category, Prototype, Factorable)\
    VOCALUBARY_SERVICE()->setFactorable(Category, Prototype, static_cast<Interface *>(Factorable))
//////////////////////////////////////////////////////////////////////////
#define VOCALUBARY_GET(Category, Prototype)\
    VOCALUBARY_SERVICE()->getFactorable(Category, Prototype)
//////////////////////////////////////////////////////////////////////////



