#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/FactorablePointer.h"
#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrototypeGeneratorInterface
        : public Mixin
    {
    public:
        virtual void setCategory( const ConstString & _category ) = 0;
        virtual const ConstString & getCategory() const = 0;
        virtual void setPrototype( const ConstString & _prototype ) = 0;
        virtual const ConstString & getPrototype() const = 0;

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual FactorablePointer generate() = 0;

    public:
        virtual uint32_t count() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrototypeGeneratorInterface> PrototypeGeneratorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PrototypeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrototypeService" )

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) = 0;
        virtual bool removePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;
        virtual const PrototypeGeneratorInterfacePtr & getGenerator( const ConstString & _category, const ConstString & _prototype ) const = 0;

    public:
        virtual FactorablePointer generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        typedef Lambda<void( const PrototypeGeneratorInterfacePtr & _factory )> LambdaPrototypeGenerator;
        virtual void foreachGenerators( const LambdaPrototypeGenerator & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PROTOTYPE_SERVICE()\
	((Mengine::PrototypeServiceInterface *)SERVICE_GET(Mengine::PrototypeServiceInterface))
//////////////////////////////////////////////////////////////////////////

