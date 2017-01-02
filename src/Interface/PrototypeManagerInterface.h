#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Factory/FactorablePtr.h"

#   include "Core/ConstString.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    class PrototypeGeneratorInterface
		: public ServantInterface
    {
	public:
		virtual bool initialize( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        virtual Factorable * generate() = 0;

        virtual uint32_t count() const = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<PrototypeGeneratorInterface> PrototypeGeneratorInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    class VisitorPrototypeGenerator
    {
    public:
        virtual void visit( const ConstString & _category, const ConstString & _type, const PrototypeGeneratorInterfacePtr & _factory ) = 0;
    };
	//////////////////////////////////////////////////////////////////////////
    class PrototypeServiceInterface
		: public ServiceInterface
    {
        SERVICE_DECLARE("PrototypeService")

    public:
        virtual bool addPrototype( const ConstString & _category, const ConstString & _prototype, const PrototypeGeneratorInterfacePtr & _generator ) = 0;
        virtual bool hasPrototype( const ConstString & _category, const ConstString & _prototype, PrototypeGeneratorInterfacePtr & _generator ) const = 0;

    public:
        virtual Factorable * generatePrototype( const ConstString & _category, const ConstString & _prototype ) = 0;

    public:
        template<class T>
        T generatePrototypeT( const ConstString & _category, const ConstString & _prototype )
        {
            Factorable * prototype = this->generatePrototype( _category, _prototype );

#   ifdef _DEBUG
            if( dynamic_cast<T>(prototype) == nullptr )
            {
                throw;
            }
#   endif

            T t = static_cast<T>(prototype);

            return t;
        }

    public:
        virtual void visitGenerators( VisitorPrototypeGenerator * _visitor ) const = 0;
    };
}

#   define PROTOTYPE_SERVICE( serviceProvider )\
	((PrototypeServiceInterface *)SERVICE_GET(serviceProvider, Menge::PrototypeServiceInterface))
    

