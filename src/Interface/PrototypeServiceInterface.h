#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/PrototypeGeneratorInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrototypeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrototypeService" )

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) = 0;
        virtual void removePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual bool hasGenerator( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr * const _generator ) const = 0;
        virtual const PrototypeGeneratorInterfacePtr & getGenerator( const ConstString & _category, const ConstString & _prototype ) const = 0;

    public:
        virtual FactorablePointer generatePrototype( const ConstString & _category, const ConstString & _prototype, const DocumentPtr & _doc ) = 0;

    public:
        typedef Lambda<void( const PrototypeGeneratorInterfacePtr & _factory )> LambdaPrototypeGenerator;
        virtual void foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PROTOTYPE_SERVICE()\
    ((Mengine::PrototypeServiceInterface *)SERVICE_GET(Mengine::PrototypeServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define PROTOTYPE_GENERATE(Category, Prototype, Document)\
    PROTOTYPE_SERVICE()->generatePrototype(Category, Prototype, Document)
//////////////////////////////////////////////////////////////////////////